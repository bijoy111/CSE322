#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/config-store-module.h"
#include <fstream>

//  Network Topology
//
//   Wifi 10.1.2.0                            
//                     AP
//      *    *    *    *
//      |    |    |    |    10.1.1.0
//  --- S    S    S    X --------------   Y    R    R    R ---
//                       point-to-point   |    |    |    |
//                                        *    *    *    *
//                                        AP
//                                          Wifi 10.1.3.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wireless high rate mobile");

int sending_packet=0;
int receiving_packet=0;
int total_packet_size=0;

//check, is the model mobile or static
static void
CourseChange(std::string context, Ptr<const MobilityModel> model)
{
    Vector position = model->GetPosition();
    //NS_LOG_UNCOND(context <<" x = " << position.x << ", y = " << position.y);
}

//necessary function for our given task(trace)
static void
Tx(Ptr<const Packet> packet)
{
    sending_packet++;
}

//necessary function for our given task(trace)
static void
Rx(Ptr<const Packet> packet, const Address &address)
{
    total_packet_size=total_packet_size+packet->GetSize(); // packet->GetSize() give the packet size in byte
    receiving_packet++;
}

//debug function. use to see the number packet drop and the time of packet drop. i specially use it to understand the trace
static void
RxDrop(Ptr<const Packet> p)
{
    //NS_LOG_UNCOND("RxDrop at " << Simulator::Now().GetSeconds());
}

//debug function(recursive). use to calculate throghput at different times
void handler()
{
    NS_LOG_UNCOND( ((receiving_packet*1024*8.0)/(Simulator::Now().GetSeconds())) <<"\t" << Simulator::Now().GetSeconds() );
    Simulator::Schedule(Seconds(1), &handler);
}

int main(int argc, char* argv[])
{
    int total_node=20;
    int total_flow=10;
    int packetPersecond=100;
    int speed=5;

    //8 data file for 8 plot. Though all are not use at once. It can possible to reduce the file number where a single file will updated accordingly adn use for more than 1 plot.
    std::string file1 = "./scratch/graphs2/data1.txt";
    std::string file2 = "./scratch/graphs2/data2.txt";
    std::string file3 = "./scratch/graphs2/data3.txt";
    std::string file4 = "./scratch/graphs2/data4.txt";
    std::string file5 = "./scratch/graphs2/data5.txt";
    std::string file6 = "./scratch/graphs2/data6.txt";
    std::string file7 = "./scratch/graphs2/data7.txt";
    std::string file8 = "./scratch/graphs2/data8.txt";

    //take inputs from command line arguments
    CommandLine cmd(__FILE__);
    cmd.AddValue ("total_node","Number of total nodes", total_node);
    cmd.AddValue ("total_flow","Number of total flows", total_flow);
    cmd.AddValue ("packetPersecond","Number of packets per second", packetPersecond);
    cmd.AddValue ("speed","Speed of nodes", speed);
    cmd.Parse(argc, argv);

    int node_per_side=total_node/2;
    int payloadSize = 1024;
    int dataRate = (payloadSize * packetPersecond * 8) / 1000;
    std::string senderDataRate = std::to_string(dataRate) + "Kbps";
    std::string bottleneckDataRate = "2Mbps";

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(1));
    Config::SetDefault("ns3::TcpL4Protocol::RecoveryType",
                       TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery")));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    //p2p nodes creation started
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(bottleneckDataRate)); // bottleneck datarate
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));  // bottleneck delay

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);
    //p2p nodes created


    //wifi nodes creation started
    NodeContainer swifiApNodes=p2pNodes.Get(0); // wifi access point node in the sending end
    NodeContainer rwifiApNodes=p2pNodes.Get(1); // wifi access point node in the recieving end

    NodeContainer swifiStaNodes; // wifi stationary nodes in the sending end
    swifiStaNodes.Create(node_per_side);
    NodeContainer rwifiStaNodes; // wifi stationary nodes in the recieving end
    rwifiStaNodes.Create(node_per_side);
     
    // Physical Layer (YANS model)
    YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default();
    channel1.AddPropagationLoss("ns3::RangePropagationLossModel");
    YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
    channel2.AddPropagationLoss("ns3::RangePropagationLossModel");
    YansWifiPhyHelper phy1;
    phy1.SetChannel(channel1.Create()); // share the same wireless medium 
    YansWifiPhyHelper phy2;
    phy2.SetChannel(channel2.Create()); // share the same wireless medium 

    // Data Link Layer
    // SSid used to set the "ssid" Attribute in the mac layer implementation
    // Each network will have a single SSID that identifies the network, 
    // and this name will be used by clients to connect to the network.
    WifiMacHelper mac1;
    WifiMacHelper mac2;
    Ssid ssid = Ssid("ns-3-ssid"); // creates an 802.11 service set identifier (SSID) 

    WifiHelper wifi1;
    WifiHelper wifi2;

    // ActiveProbing false -  probe requests will not be sent by MACs created by this
    // helper, and stations will listen for AP beacons.
    NetDeviceContainer sstaDevices;
    NetDeviceContainer rstaDevices;
    mac1.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    mac2.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    sstaDevices = wifi1.Install(phy1, mac1, swifiStaNodes);
    rstaDevices = wifi2.Install(phy2, mac2, rwifiStaNodes);
    NetDeviceContainer sApDevices;
    NetDeviceContainer rApDevices;
    mac1.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    mac2.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    sApDevices = wifi1.Install(phy1, mac1, swifiApNodes);
    rApDevices = wifi2.Install(phy2, mac2, rwifiApNodes);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(1.0),
                                  "DeltaY",
                                  DoubleValue(1.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(swifiApNodes);
    mobility.Install(rwifiApNodes);

    mobility.SetMobilityModel(
        "ns3::RandomWalk2dMobilityModel",
        "Bounds",
        RectangleValue(Rectangle(-50, 50, -50, 50)),
        "Speed",
        StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(speed) + "]"));
    mobility.Install(swifiStaNodes);
    mobility.Install(rwifiStaNodes);
    
    InternetStackHelper stack;
    stack.Install(swifiStaNodes);
    stack.Install(rwifiStaNodes);
    stack.Install(swifiApNodes);
    stack.Install(rwifiApNodes);

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer swifiStaInterfaces=address.Assign(sstaDevices);
    address.Assign(sApDevices);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer rwifiStaInterfaces=address.Assign(rstaDevices);
    address.Assign(rApDevices);

    int sr_num=0;
    for (int i = 0; i <total_flow; i++)
    {
        uint16_t port = 5000 + i;
        sr_num=sr_num%node_per_side;

        OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address(InetSocketAddress(rwifiStaInterfaces.GetAddress(sr_num), port)));
        onOffHelper.SetAttribute("PacketSize", UintegerValue(payloadSize));
        onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate(senderDataRate)));
        ApplicationContainer s_App = onOffHelper.Install(swifiStaNodes.Get(sr_num));
        s_App.Start(Seconds(1.0));
        s_App.Stop(Seconds(8.0));

        Ptr<OnOffApplication> sender = DynamicCast<OnOffApplication>(s_App.Get(0));
        sender->TraceConnectWithoutContext("Tx", MakeCallback(&Tx));


        PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer r_App = packetSinkHelper.Install(rwifiStaNodes.Get(sr_num));
        r_App.Start(Seconds(2.0));
        r_App.Stop(Seconds(10.0));

        Ptr<PacketSink> reciever = DynamicCast<PacketSink>(r_App.Get(0));
        reciever->TraceConnectWithoutContext("Rx", MakeCallback(&Rx));

        sr_num++;
    }
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    //use for debugging
    /*
    Config::SetDefault ("ns3::ConfigStore::Filename", StringValue
    ("output-attributes.txt"));
    Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue
    ("RawText"));
    Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
    ConfigStore outputConfig2;
    outputConfig2.ConfigureDefaults ();
    outputConfig2.ConfigureAttributes ();
    */
    
    //Simulator::Schedule(Seconds(1), &handler);

    Simulator::Run();

    std::ofstream MyFile1(file1, std::ios_base::app);
    //MyFile1<<total_node<<" "<<(receiving_packet*payloadSize*8.0)/(Simulator::Now().GetSeconds())<<std::endl;
    MyFile1<<total_node<<" "<<(total_packet_size*8.0)/(Simulator::Now().GetSeconds())<<std::endl;
    MyFile1.close();

    std::ofstream MyFile2(file2, std::ios_base::app);
    MyFile2<<total_node<<" "<<((receiving_packet*1.0)/sending_packet)<<std::endl;
    MyFile2.close();

    std::ofstream MyFile3(file3, std::ios_base::app);
    MyFile3<<total_flow<<" "<<(total_packet_size*8.0)/(Simulator::Now().GetSeconds())<<std::endl;
    MyFile3.close();

    std::ofstream MyFile4(file4, std::ios_base::app);
    MyFile4<<total_flow<<" "<<((receiving_packet*1.0)/sending_packet)<<std::endl;
    MyFile4.close();

    std::ofstream MyFile5(file5, std::ios_base::app);
    MyFile5<<packetPersecond<<" "<<(total_packet_size*8.0)/(Simulator::Now().GetSeconds())<<std::endl;
    MyFile5.close();

    std::ofstream MyFile6(file6, std::ios_base::app);
    MyFile6<<packetPersecond<<" "<<((receiving_packet*1.0)/sending_packet)<<std::endl;
    MyFile6.close();

    std::ofstream MyFile7(file7, std::ios_base::app);
    MyFile7<<speed<<" "<<(total_packet_size*8.0)/(Simulator::Now().GetSeconds())<<std::endl;
    MyFile7.close();

    std::ofstream MyFile8(file8, std::ios_base::app);
    MyFile8<<speed<<" "<<((receiving_packet*1.0)/sending_packet)<<std::endl;
    MyFile8.close();

    Simulator::Destroy();
    return 0;
}
//  ./"scratch/1905052_2.sh"