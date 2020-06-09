#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/point-to-point-remote-channel.h"
#include "loss-helper.h"
#include "../model/loss-net-device.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/mpi-receiver.h"
#include "ns3/mpi-interface.h"
#include "ns3/log.h"
#include "ns3/ptr.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LossHelper");

LossHelper::LossHelper()
{
    // m_loss = lossRate;

    m_queueFactory.SetTypeId("ns3::DropTailQueue<Packet>");
    m_switchDeviceFactory.SetTypeId("ns3::LossNetDevice");
    m_channelFactory.SetTypeId("ns3::PointToPointChannel");
    m_remoteChannelFactory.SetTypeId("ns3::PointToPointRemoteChannel");
    m_endDeviceFactory.SetTypeId("ns3::PointToPointNetDevice");

    NS_LOG_FUNCTION(this);
}

LossHelper::~LossHelper()
{
    NS_LOG_FUNCTION(this);
}

NetDeviceContainer LossHelper::Install(Ptr<Node> end,Ptr<Node> s){
    NetDeviceContainer container;

    Ptr<PointToPointNetDevice> dev_end = m_endDeviceFactory.Create<PointToPointNetDevice>();
    dev_end->SetAddress(Mac48Address::Allocate());
    Ptr<Queue<Packet>> queueA = m_queueFactory.Create<Queue<Packet>>();
    dev_end->SetQueue(queueA);
    end->AddDevice(dev_end);

    Ptr<LossNetDevice> dev_switch = m_switchDeviceFactory.Create<LossNetDevice>();
    // devB->SetLossRate(m_loss);
    dev_switch->SetAddress(Mac48Address::Allocate());
    Ptr<Queue<Packet>> queueB = m_queueFactory.Create<Queue<Packet>>();
    dev_switch->SetQueue(queueB);
    s->AddDevice(dev_switch);

    bool useNormalChannel = true;
    Ptr<PointToPointChannel> channel = 0;
    if (MpiInterface::IsEnabled())
    {
        uint32_t n1SystemId = end->GetSystemId();
        uint32_t n2SystemId = s->GetSystemId();
        uint32_t currSystemId = MpiInterface::GetSystemId();
        if (n1SystemId != currSystemId || n2SystemId != currSystemId)
        {
            useNormalChannel = false;
        }
    }
    if (useNormalChannel)
    {
        channel = m_channelFactory.Create<PointToPointChannel>();
    }
    else
    {
        channel = m_remoteChannelFactory.Create<PointToPointRemoteChannel>();
        Ptr<MpiReceiver> mpiRecA = CreateObject<MpiReceiver>();
        Ptr<MpiReceiver> mpiRecB = CreateObject<MpiReceiver>();
        mpiRecA->SetReceiveCallback(MakeCallback(&PointToPointNetDevice::Receive, dev_end));
        mpiRecB->SetReceiveCallback(MakeCallback(&LossNetDevice::Receive, dev_switch));
        dev_end->AggregateObject(mpiRecA);
        dev_switch->AggregateObject(mpiRecB);
    }

    dev_end->Attach(channel);
    dev_switch->Attach(channel);
    container.Add(dev_end);
    container.Add(dev_switch);
    return container;

}

void LossHelper::SetDeviceAttribute(std::string n1, const AttributeValue &v1){

    m_endDeviceFactory.Set(n1,v1);
    m_switchDeviceFactory.Set(n1,v1);
}

void LossHelper::SetChannelAttribute(std::string n1, const AttributeValue &v1)
{
    m_channelFactory.Set(n1, v1);
}

// NetDeviceContainer LossHelper::Install(NodeContainer c)
// {
//     Ptr<Node> a = c.Get(0);
//     Ptr<Node> b = c.Get(1);
//     NetDeviceContainer container;

//     Ptr<LossNetDevice> devA = m_switchDeviceFactory.Create<LossNetDevice>();
//     // devA->SetLossRate(m_loss);
//     devA->SetAddress(Mac48Address::Allocate());
//     a->AddDevice(devA);
//     Ptr<Queue<Packet>> queueA = m_queueFactory.Create<Queue<Packet>>();
//     devA->SetQueue(queueA);

//     Ptr<LossNetDevice> devB = m_switchDeviceFactory.Create<LossNetDevice>();
//     // devB->SetLossRate(m_loss);
//     devB->SetAddress(Mac48Address::Allocate());
//     b->AddDevice(devB);
//     Ptr<Queue<Packet>> queueB = m_queueFactory.Create<Queue<Packet>>();
//     devB->SetQueue(queueB);

//     bool useNormalChannel = true;
//     Ptr<PointToPointChannel> channel = 0;
//     if (MpiInterface::IsEnabled())
//     {
//         uint32_t n1SystemId = a->GetSystemId();
//         uint32_t n2SystemId = b->GetSystemId();
//         uint32_t currSystemId = MpiInterface::GetSystemId();
//         if (n1SystemId != currSystemId || n2SystemId != currSystemId)
//         {
//             useNormalChannel = false;
//         }
//     }
//     if (useNormalChannel)
//     {
//         channel = m_channelFactory.Create<PointToPointChannel>();
//     }
//     else
//     {
//         channel = m_remoteChannelFactory.Create<PointToPointRemoteChannel>();
//         Ptr<MpiReceiver> mpiRecA = CreateObject<MpiReceiver>();
//         Ptr<MpiReceiver> mpiRecB = CreateObject<MpiReceiver>();
//         mpiRecA->SetReceiveCallback(MakeCallback(&LossNetDevice::Receive, devA));
//         mpiRecB->SetReceiveCallback(MakeCallback(&LossNetDevice::Receive, devB));
//         devA->AggregateObject(mpiRecA);
//         devB->AggregateObject(mpiRecB);
//     }

//     devA->Attach(channel);
//     devB->Attach(channel);
//     container.Add(devA);
//     container.Add(devB);
//     return container;
// }


} // namespace ns3
