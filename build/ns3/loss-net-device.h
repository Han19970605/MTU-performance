#ifndef LOSSNETDEVICE_H
#define LOSSNETDEVICE_H

/**
 * lossnetdevice:
 * 1.inherited from pointtopointdevice
 * 2.add loss rate
 * 3.use for modify mtu experiment to simulate loss rate
*/

#include "ns3/point-to-point-net-device.h"

namespace ns3
{
class LossNetDevice : public PointToPointNetDevice
{
public:
    static TypeId GetTypeId();

    LossNetDevice();

    virtual ~LossNetDevice();

    virtual bool Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);

    bool SetLossRate(uint16_t rate);

    uint16_t GetLossRate();

private:
    //最大值是1000即最低丢包率是0.1 设置为10丢包率为百分之1
    uint16_t lossRate;
    TracedCallback<Ptr<const Packet>> m_randomLoss;
};

} // namespace ns3

#endif