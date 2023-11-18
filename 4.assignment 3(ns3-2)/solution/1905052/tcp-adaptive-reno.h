#ifndef TCP_ADAPTIVERENO_H
#define TCP_ADAPTIVERENO_H

#include "tcp-congestion-ops.h"

#include "ns3/data-rate.h"
#include "ns3/event-id.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/traced-value.h"
#include "tcp-westwood-plus.h"

namespace ns3
{
    class Time;
    class EventId;
    class Packet;
    class TcpHeader;
    
    class TcpAdaptiveReno : public TcpWestwoodPlus
    {
    public:

        static TypeId GetTypeId();

        TcpAdaptiveReno();

        TcpAdaptiveReno(const TcpAdaptiveReno& sock);
        virtual ~TcpAdaptiveReno();

        enum FilterType
        {
            NONE,
            TUSTIN
        };

        virtual uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight);

        virtual void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked, const Time& rtt);

        virtual Ptr<TcpCongestionOps> Fork();

    private:

        double EstimateCongestionLevel();
        void EstimateIncWnd(Ptr<TcpSocketState> tcb);
        void EstimateBW (const Time& rtt, Ptr<TcpSocketState> tcb);

    protected:

        virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

        Time    RTT_cong;
        Time    curr_Rtt;
        Time    RTT_j;
        Time    min_Rtt;
        Time    RTT_cong_prev;

        uint32_t    W_base;
        int32_t     W_inc;
        int32_t     W_probe;
    };
}

#endif