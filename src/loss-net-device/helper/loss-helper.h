
// #include "ns3/network-module.h"
// #include "ns3/point-to-point-module.h"
// #include "ns3/internet-module.h"
#include "ns3/object-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"

namespace ns3
{
class LossHelper : public Object
{
public:
    LossHelper();
    ~LossHelper();

    NetDeviceContainer Install(NodeContainer c);
    NetDeviceContainer Install(Ptr<Node> end,Ptr<Node> s);
    // void SetEndDeviceAttribute(std::string n1, const AttributeValue &v1);
    // void SetDeviceAttribute(std::string n1, const AttributeValue &v1);
    void SetDeviceAttribute(std::string n1, const AttributeValue &v1);
    void SetChannelAttribute(std::string n1, const AttributeValue &v1);

private:
    // uint16_t m_loss;
    ObjectFactory m_queueFactory;
    ObjectFactory m_channelFactory;
    ObjectFactory m_switchDeviceFactory;
    ObjectFactory m_endDeviceFactory;
    ObjectFactory m_remoteChannelFactory;
};
} // namespace ns3