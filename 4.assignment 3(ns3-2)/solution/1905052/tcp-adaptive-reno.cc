#include "tcp-adaptive-reno.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("TcpAdaptiveReno");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(TcpAdaptiveReno);

    TypeId
    TcpAdaptiveReno::GetTypeId()
    {
        static TypeId tid =
            TypeId("ns3::TcpAdaptiveReno")
                .SetParent<TcpNewReno>()
                .SetGroupName("Internet")
                .AddConstructor<TcpAdaptiveReno>()
                .AddAttribute(
                    "FilterType",
                    "Use this to choose no filter or Tustin's approximation filter",
                    EnumValue(TcpAdaptiveReno::TUSTIN),
                    MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                    MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
                .AddTraceSource("EstimatedBW",
                                "The estimated bandwidth",
                                MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                                "ns3::TracedValueCallback::DataRate");
        return tid;
    }

    TcpAdaptiveReno::TcpAdaptiveReno()
    : TcpWestwoodPlus(),
      RTT_cong (Time (0)),
      curr_Rtt (Time (0)),
      RTT_j (Time (0)),
      min_Rtt (Time (0)),
      RTT_cong_prev (Time(0)),

      W_base (0),
      W_inc (0),
      W_probe (0)
    {
        NS_LOG_FUNCTION(this);
    }

    TcpAdaptiveReno::TcpAdaptiveReno(const TcpAdaptiveReno& sock)
    : TcpWestwoodPlus(sock),
      RTT_cong (Time (0)),
      curr_Rtt (Time (0)),
      RTT_j (Time (0)),
      min_Rtt (Time (0)),
      RTT_cong_prev (Time(0)),

      W_base (0),
      W_inc (0),
      W_probe (0)
    {
        NS_LOG_FUNCTION(this);
        NS_LOG_LOGIC("Invoked the copy constructor");
    }

    TcpAdaptiveReno::~TcpAdaptiveReno()
    {
    }


    void
    TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked, const Time& rtt)
    {
        NS_LOG_FUNCTION(this << tcb << packetsAcked << rtt);

        if (rtt.IsZero())
        {
            NS_LOG_WARN("RTT measured is zero!");
            return;
        }

        m_ackedSegments += packetsAcked;

        curr_Rtt=rtt;

        if(min_Rtt.IsZero())
        {
            min_Rtt=rtt;
        }
        else if(min_Rtt>rtt)
        {
            min_Rtt=rtt;
        }
        
        NS_LOG_LOGIC("curr_Rtt: "<<curr_Rtt.GetMilliSeconds()<<"ms");
        NS_LOG_LOGIC("min_Rtt: "<<min_Rtt.GetMilliSeconds()<<"ms");

        TcpWestwoodPlus::EstimateBW(rtt,tcb);
    }

    double
    TcpAdaptiveReno::EstimateCongestionLevel()
    {
        double a = 0.85; // an exponential smoothing factor
        if(min_Rtt>RTT_cong_prev)
        {
            a=0;
        }
        
        double tmp_conj = (a * RTT_cong_prev.GetSeconds()) + ((1-a) * RTT_j.GetSeconds());
        RTT_cong = Seconds(tmp_conj);
        NS_LOG_LOGIC("Rtt_cong : " << RTT_cong << " ; Rtt_cong_prev : " << RTT_cong_prev << " ; RTT_j : " << RTT_j);

        double c = std::min((curr_Rtt.GetSeconds() - min_Rtt.GetSeconds()) / (tmp_conj - min_Rtt.GetSeconds()),1.0);
        return c;
    }


    void 
    TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
    {
        double c_level = EstimateCongestionLevel(); //  estimated congestion level calculated in the previous function.

        int M = 1000; // set M = 1000.

        double W_inc_max = (m_currentBW.Get().GetBitRate()) / (M * static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize)); 

        double alp = 10;

        double bita = 2 * W_inc_max * ((1/alp) - ((1/alp + 1)/(std::exp(alp))));

        double gama = 1 - (2 * W_inc_max * ((1/alp) - ((1/alp + 0.5)/(std::exp(alp)))));
        
        W_inc = (int)((W_inc_max / std::exp(alp * c_level)) + (bita * c_level) + gama);

        NS_LOG_LOGIC ("Congestion level: " << c_level);
        NS_LOG_LOGIC ("RTT_cong: " << RTT_cong.GetMilliSeconds () << "ms");
        NS_LOG_LOGIC ("min_Rtt: " << min_Rtt.GetMilliSeconds () << "ms");
        NS_LOG_LOGIC ("W_inc: " << W_inc);
        NS_LOG_LOGIC("W_inc: "<<W_inc<<" ; prev_wind: "<<tcb->m_cWnd<<" ; new: "<<(W_inc / (int)tcb->m_cWnd));
        NS_LOG_LOGIC("maxInc: "<<W_inc_max<<"; congestion_level: "<<c_level<<" ; bita: "<<bita<<" ; gama: "<<gama<<" ; exp(alp * c_level): "<<std::exp(alp * c_level));   
    }

    void
    TcpAdaptiveReno::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
    {
        NS_LOG_FUNCTION(this << tcb << segmentsAcked);

        if (segmentsAcked > 0)
        {
            EstimateIncWnd(tcb);
            
            double tmp = static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get ();

            tmp = std::max (1.0, tmp);

            W_base += static_cast<uint32_t> (tmp);

            W_probe = std::max((double) (W_probe + W_inc / (int)tcb->m_cWnd.Get()), (double) 0);
            
            NS_LOG_LOGIC("Before "<<tcb->m_cWnd<< " ; base "<<W_base<<" ; probe "<<W_probe);

            tcb->m_cWnd = W_base + W_probe;

            NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<" ssthresh " << tcb->m_ssThresh);
        }
    }

    uint32_t
    TcpAdaptiveReno::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
    {
        RTT_cong_prev = RTT_cong; // update

        RTT_j = curr_Rtt; // update

        double c = EstimateCongestionLevel();

        uint32_t ssthresh = std::max (2*tcb->m_segmentSize,(uint32_t) (tcb->m_cWnd / (1.0+c)));

        W_base = ssthresh;

        W_probe = 0;
    
        NS_LOG_LOGIC("new ssthresh : "<<ssthresh<<" ; old conj Rtt : "<<RTT_cong_prev<<" ; new conj Rtt : "<<RTT_cong<<" ; cong : "<<c);
    
        return ssthresh;
    }

    Ptr<TcpCongestionOps>
    TcpAdaptiveReno::Fork()
    {
        return CreateObject<TcpAdaptiveReno>(*this);
    }
}