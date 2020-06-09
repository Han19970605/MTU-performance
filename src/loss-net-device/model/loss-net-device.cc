#include "loss-net-device.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include <cstdlib>

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("LossNetDevice");
    NS_OBJECT_ENSURE_REGISTERED(LossNetDevice);

    TypeId LossNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::LossNetDevice")
                                .SetParent<PointToPointNetDevice>()
                                .SetGroupName("LossNetDevice")
                                .AddConstructor<LossNetDevice>()
                                .AddAttribute("LossRate", "from 0-1000. loss rate equals set rate /10",
                                              UintegerValue(0), MakeUintegerAccessor(&LossNetDevice::lossRate),
                                              MakeUintegerChecker<uint16_t>());
        // .AddAttribute("LossRate", "Sender loss rate",
        //               UintegerValue(0), MakeDataRateAccessor(&MMtuNetDevice::lossRate));
        return tid;
    }

    LossNetDevice::LossNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    LossNetDevice::~LossNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    bool LossNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        uint16_t ifLoss = rand() % 1000;
        // std::cout<<"imhere"<<ifLoss<<std::endl;
        if (ifLoss != 0 && ifLoss <= lossRate)
        {
            // std::cout << "loss happens" << ifLoss << std::endl;
            m_randomLoss(packet);
            return true;
        }
        else
        {
            return PointToPointNetDevice::Send(packet, dest, protocolNumber);
        }
    }

    bool LossNetDevice::SetLossRate(uint16_t rate)
    {
        lossRate = rate;
        return true;
    }

    uint16_t LossNetDevice::GetLossRate()
    {
        return lossRate;
    }
} // namespace ns3