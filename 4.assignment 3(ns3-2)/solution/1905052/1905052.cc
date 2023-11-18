#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"
#include "ns3/point-to-point-layout-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task 1");


//taken from tutorial-app.h & tutorial-app.c
class Task1App : public Application
{
  public:
    Task1App();
    ~Task1App() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Setup the socket.
     * \param socket The socket.
     * \param address The destination address.
     * \param packetSize The packet size to transmit.
     * \param nPackets The number of packets to transmit.
     * \param dataRate the data rate to use.
     */
    void Setup(Ptr<Socket> socket,
               Address address,
               uint32_t packetSize,
               DataRate dataRate,
               uint32_t simultime);

  private:
    void StartApplication() override;
    void StopApplication() override;

    /// Schedule a new transmission.
    void ScheduleTx();
    /// Send a packet.
    void SendPacket();

    Ptr<Socket> m_socket;   //!< The transmission socket.
    Address m_peer;         //!< The destination address.
    uint32_t m_packetSize;  //!< The packet size.
    DataRate m_dataRate;    //!< The data rate to use.
    EventId m_sendEvent;    //!< Send event.
    bool m_running;         //!< True if the application is running.
    uint32_t m_packetsSent; //!< The number of packets sent.
    uint32_t m_simultime;
};

Task1App::Task1App()
    : m_socket(nullptr),
      m_peer(),
      m_packetSize(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0),
      m_simultime (0)
{
}

Task1App::~Task1App()
{
    m_socket = 0;
}

/* static */
TypeId
Task1App::GetTypeId()
{
    static TypeId tid = TypeId("Task1App")
                            .SetParent<Application>()
                            .SetGroupName("Tutorial")
                            .AddConstructor<Task1App>();
    return tid;
}

void
Task1App::Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   DataRate dataRate,
                   uint32_t simultime)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_dataRate = dataRate;
    m_simultime = simultime;
}

void
Task1App::StartApplication()
{
    m_running = true;
    m_packetsSent = 0;
    if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
    else
    {
      m_socket->Bind6 ();
    }
    m_socket->Connect(m_peer);
    SendPacket();
}

void
Task1App::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
Task1App::SendPacket()
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    m_socket->Send(packet);

    if(Simulator::Now().GetSeconds() < m_simultime) ScheduleTx();
}

void
Task1App::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &Task1App::SendPacket, this);
    }
}
//upto previous line these code taken from tutorial-app.h & tutorial-app.c

//this CwndChange function is taken from seventh.cc
static void
CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << newCwnd);
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newCwnd << std::endl;
}


int main(int argc, char* argv[])
{

    std::string file1;
    std::string file2;
    std::string file3;
    std::string file4;
    std::string file5;

    int nleaf = 2; // red-vs-fengadadaptive.cc
    int nflow = 2;
    uint32_t payloadSize = 1024;
    int simulationTimeInSeconds = 40;
    int cleanupTime = 2;
    std::string senderDataRate = "1Gbps";
    int bottlelinkdelay=100;
    int bottlelinkrate=50;
    int power = 6;

    int task=1;
    std::string tcpVariant1 = "ns3::TcpNewReno";
    std::string tcpVariant2 = "ns3::TcpHighSpeed";

    //take inputs from command line arguments
    CommandLine cmd(__FILE__);
    cmd.AddValue ("nleaf","Number of total nodes", nleaf);
    cmd.AddValue ("bottlelinkrate","Bottle Link Data Rate", bottlelinkrate);
    cmd.AddValue ("power","Power for calculating Packet loss rate", power);

    cmd.AddValue ("task","Task number", task);
    cmd.AddValue ("tcpVariant1","Congestion algorithm for flow 1", tcpVariant1);
    cmd.AddValue ("tcpVariant2","Congestion algorithm for flow 2", tcpVariant2);
    cmd.Parse(argc, argv);


    if(task==1)
    {
        file1 = "./scratch/Task1/TP_vs_BDT/data1.txt";
        file2 = "./scratch/Task1/TP_vs_BDT/data2.txt";

        file3 = "./scratch/Task1/TP_vs_PLR/data3.txt";
        file4 = "./scratch/Task1/TP_vs_PLR/data4.txt";

        file5 = "./scratch/Task1/CW_vs_Time";
    }
    else if(task==2)
    {
        file1 = "./scratch/Task2/TP_vs_BDT/data1.txt";
        file2 = "./scratch/Task2/TP_vs_BDT/data2.txt";

        file3 = "./scratch/Task2/TP_vs_PLR/data3.txt";
        file4 = "./scratch/Task2/TP_vs_PLR/data4.txt";

        file5 = "./scratch/Task2/CW_vs_Time";
    }

    nflow=nleaf;
    std::string bottleNeckDataRate = std::to_string(bottlelinkrate) + "Mbps";
    std::string bottleNeckDelay = std::to_string(bottlelinkdelay) + "ms" ;
    double packet_loss_rate = (1.0 / std::pow(10, power));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));


    PointToPointHelper bottleneck_link;
    bottleneck_link.SetDeviceAttribute  ("DataRate", StringValue(bottleNeckDataRate));
    bottleneck_link.SetChannelAttribute ("Delay", StringValue(bottleNeckDelay));

    PointToPointHelper p2p_sr;
    p2p_sr.SetDeviceAttribute("DataRate", StringValue("1Gbps")); // datarate of sender and receiver
    p2p_sr.SetChannelAttribute("Delay", StringValue("1ms"));  // delay of sender and receiver
    p2p_sr.SetQueue ("ns3::DropTailQueue", "MaxSize",
    StringValue (std::to_string (bottlelinkdelay*bottlelinkrate) + "p"));

    PointToPointDumbbellHelper d(nleaf, p2p_sr, nleaf, p2p_sr, bottleneck_link); // red-vs-fengadadaptive.cc

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>(); // tracing.rst
    em->SetAttribute("ErrorRate", DoubleValue(packet_loss_rate)); // tracing.rst
    d.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em)); // tracing.rst


    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpVariant1));

    InternetStackHelper stack; // code taken from red-vs-fengadadaptive.cc
    for (uint32_t i = 0; i < d.LeftCount(); i+=2)
    {
        stack.Install(d.GetLeft(i));
    }
    for (uint32_t i = 0; i < d.RightCount(); i+=2)
    {
        stack.Install(d.GetRight(i));
    }

    stack.Install(d.GetLeft());
    stack.Install(d.GetRight());

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpVariant2));

    InternetStackHelper stack1; // code taken from red-vs-fengadadaptive.cc
    for (uint32_t i = 1; i < d.LeftCount(); i+=2)
    {
        stack1.Install(d.GetLeft(i));
    }
    for (uint32_t i = 1; i < d.RightCount(); i+=2)
    {
        stack1.Install(d.GetRight(i));
    }

    d.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"),
                          Ipv4AddressHelper("10.2.1.0", "255.255.255.0"),
                          Ipv4AddressHelper("10.3.1.0", "255.255.255.0"));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); // from third.cc

    FlowMonitorHelper flowmon; // traffic-control.cc

    flowmon.SetMonitorAttribute("MaxPerHopDelay",TimeValue(Seconds(cleanupTime))); // wifi-olsr-flowmon.py , and MaxPerHopDelay is defined in output-attributes.txt

    Ptr<FlowMonitor> monitor = flowmon.InstallAll(); // traffic-control.cc

    uint16_t port = 5000;

    for(int i=0;i<nflow;i++)
    {
        Address sinkAddress (InetSocketAddress (d.GetRightIpv4Address (i), port));
        PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApps = packetSinkHelper.Install (d.GetRight (i));
        sinkApps.Start (Seconds (0));
        sinkApps.Stop (Seconds (simulationTimeInSeconds+cleanupTime));

        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (d.GetLeft (i), TcpSocketFactory::GetTypeId ());

        Ptr<Task1App> app = CreateObject<Task1App> ();
        app->Setup (ns3TcpSocket, sinkAddress, payloadSize, DataRate (senderDataRate),simulationTimeInSeconds);

        d.GetLeft (i)->AddApplication (app);
        app->SetStartTime (Seconds (1));
        app->SetStopTime (Seconds (simulationTimeInSeconds));


        std::ostringstream oss;
        oss << file5 << "/flow" << i+1 <<  ".txt";
        AsciiTraceHelper asciiTraceHelper;
        Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (oss.str());
        ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
    }


    Simulator::Stop (Seconds (simulationTimeInSeconds+cleanupTime));
    Simulator::Run ();
    

    
    std::ofstream MyFile1(file1, std::ios_base::app);
    std::ofstream MyFile2(file2, std::ios_base::app);

    std::ofstream MyFile3(file3, std::ios_base::app);
    std::ofstream MyFile4(file4, std::ios_base::app);

    double throughput1=0;
    double throughput2=0;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    int tmpp=1;

    for (auto iter = stats.begin (); iter != stats.end (); ++iter)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first); 
        double throughput=iter->second.rxBytes * 8.0/((simulationTimeInSeconds+cleanupTime)*1000);

        if(tmpp%2==1)
        {
            throughput1+=throughput;
        }
        else if(tmpp%2==0)
        {
            throughput2+=throughput;
        }
        
        tmpp++;
    }

    throughput1=(throughput1*1.0)/nflow;
    throughput2=(throughput2*1.0)/nflow;

    MyFile1<<bottlelinkrate<<" "<<throughput1<<std::endl;
    MyFile2<<bottlelinkrate<<" "<<throughput2<<std::endl;

    MyFile3<<power<<" "<<throughput1<<std::endl;
    MyFile4<<power<<" "<<throughput2<<std::endl;


    Simulator::Destroy ();

    return 0;
}