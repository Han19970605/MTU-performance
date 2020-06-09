#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/loss-net-device-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ptr.h"
#include "ns3/node.h"

#define START_TIME 0.0
#define BUFFER_SIZE 134217728
#define MSS MTU - 40

NS_LOG_COMPONENT_DEFINE("NEW_NETDEVICE");

// #define END_TIME 2.0
// #define LOSS_RATE 10
// #define DELAY "2ms"
// #define DATA_RATE "16Mbps"
// #define MTU 1500

uint32_t ComputeRto(uint16_t baseRTT, uint16_t transTime, uint16_t switchNumber)
{
    //unit is microseconds
    uint32_t Rto = baseRTT * 3 + transTime * switchNumber;
    return Rto;
}

using namespace ns3;

int main(int argc, char *argv[])
{
    //日志组件生效
    LogComponentEnable("NEW_NETDEVICE", LOG_INFO);

    /**
     * 延迟`带宽`拥塞算法
     * 丢包率`结束时间`最大传输单元`拥塞避免阈值`RTO
     * cmd读出的值
    */
    std::string FileName = "data";
    std::string DELAY = "5us";
    uint32_t MTU = 800;
    uint16_t LOSS_RATE = 100;
    std::string DATA_RATE = "16Mbps";
    std::string TCP_PROTOCOL = "ns3::TcpNewReno";

    //计算得出的值
    uint32_t THRESHOLD = 0;
    uint32_t RTO_TIME = 1;
    uint64_t TRANS_DELAY = 0;
    uint64_t END_TIME = 20;

    //脚本传输的参数
    CommandLine cmd;
    cmd.AddValue("DELAY", "延迟", DELAY);
    cmd.AddValue("MTU", "最大传输单元", MTU);
    cmd.AddValue("LOSS_RATE", "丢包率", LOSS_RATE);
    cmd.AddValue("DATA_RATE", "传输速率", DATA_RATE);
    cmd.AddValue("TCP_PROTOCOL", "PROTOCOL", TCP_PROTOCOL);
    cmd.AddValue("FileName", "store data to file", FileName);
    cmd.Parse(argc, argv);

    /**
     * 计算拥塞拥塞避免阶段的阈值
     * 计算每个节点的发送延迟
     * 计算最小重传时间
    */
    if (DATA_RATE == "16Mbps")
    {
        THRESHOLD = 8 * 1024 * 1024;
    }
    else if (DATA_RATE == "128Mbps")
    {
        THRESHOLD = 64 * 1024 * 1024;
    }
    else if (DATA_RATE == "1Gbps")
    {
        THRESHOLD = 512 * 1024 * 1024;
    }

    std::cout << MTU << "," << THRESHOLD << std::endl;
    TRANS_DELAY = float(MTU) * 8 / (float)THRESHOLD / 2 * 1000000;
    std::cout << TRANS_DELAY << std::endl;

    if (DELAY == "5us")
    {
        RTO_TIME = ComputeRto(5, TRANS_DELAY, 1);
        END_TIME = (TRANS_DELAY * 2 + 5 * 4) * 20000;
        std::cout << END_TIME << std::endl;
    }
    else if (DELAY == "2ms")
    {
        RTO_TIME = ComputeRto(2000, TRANS_DELAY, 1);
        END_TIME = (TRANS_DELAY * 2 + 2000 * 4) * 20000;
    }
    else if (DELAY == "50ms")
    {
        RTO_TIME = ComputeRto(50000, TRANS_DELAY, 1);
        END_TIME = (TRANS_DELAY * 2 + 50000 * 4) * 20000;
        std::cout << END_TIME << std::endl;
    }

    //设置时间分辨率
    Time::SetResolution(Time::NS);

    //日志组件生效
    // LogComponentEnable("MTU_BULKSEND", LOG_INFO);
    // LogComponentEnable("BulkSendApplication", LOG_INFO);
    // LogComponentEnable("PacketSink", LOG_INFO);

    //配置
    //advertised window size
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(BUFFER_SIZE));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(BUFFER_SIZE));
    //mtu在网卡上配置
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(MSS));
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue(TCP_PROTOCOL));
    Config::SetDefault("ns3::LossNetDevice::LossRate", UintegerValue(LOSS_RATE));
    //慢启动的阈值 设为满带宽的一半
    Config::SetDefault("ns3::TcpSocket::InitialSlowStartThreshold", UintegerValue(THRESHOLD));
    Config::SetDefault("ns3::TcpSocketBase::MinRto", TimeValue(MicroSeconds(RTO_TIME)));

    /*
    TOPOLOGY
    h0--s0--h1
     */

    //生成网络节点
    NodeContainer ends, switches;
    ends.Create(2);
    switches.Create(1);

    //安装协议栈
    InternetStackHelper stack;
    stack.Install(ends);
    stack.Install(switches);

    NodeContainer h0s0 = NodeContainer(ends.Get(0), switches.Get(0));
    NodeContainer h1s0 = NodeContainer(ends.Get(1), switches.Get(0));

    LossHelper lossHelper = LossHelper();
    lossHelper.SetChannelAttribute("Delay", StringValue(DELAY));
    lossHelper.SetDeviceAttribute("DataRate", StringValue(DATA_RATE));
    lossHelper.SetDeviceAttribute("Mtu", UintegerValue(MTU));

    NetDeviceContainer d_h0s0 = lossHelper.Install(h0s0.Get(0), h0s0.Get(1));
    NetDeviceContainer d_h1s0 = lossHelper.Install(h1s0.Get(0), h1s0.Get(1));

    //分配ip
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i_h0s0 = ipv4.Assign(d_h0s0);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i_h1s0 = ipv4.Assign(d_h1s0);

    //发送
    BulkSendHelper bulk = BulkSendHelper("ns3::TcpSocketFactory",
                                         InetSocketAddress(i_h1s0.GetAddress(0), 9));
    bulk.SetAttribute("SendSize", UintegerValue(MSS));
    ApplicationContainer bulksend = bulk.Install(h0s0.Get(0));

    //接受
    PacketSinkHelper sink = PacketSinkHelper("ns3::TcpSocketFactory",
                                             InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApps = sink.Install(h1s0.Get(0));

    sinkApps.Start(MicroSeconds(START_TIME));
    sinkApps.Stop(MicroSeconds(END_TIME));
    bulksend.Start(MicroSeconds(START_TIME));
    bulksend.Stop(MicroSeconds(END_TIME));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Ptr<PacketSink> packetsink = DynamicCast<PacketSink>(sinkApps.Get(0));

    Simulator::Stop(MicroSeconds(END_TIME));
    Simulator::Run();

    //write data
    std::string fileName = "./data/";
    fileName = fileName.append(FileName);
    std::ofstream file(fileName, std::ios::app);
    file << DELAY << "," << LOSS_RATE << "," << packetsink->GetTotalRx() << "," << MTU << ","
         << END_TIME << ","
         << "SIMPLE"
         << "," << TCP_PROTOCOL
         << "," << DATA_RATE
         << "\n";
    NS_LOG_INFO("delay is:" << DELAY << ","
                            << "totalRx is:" << packetsink->GetTotalRx() << ","
                            << "mtu is:" << MTU << ","
                            << "mss is" << MSS << ","
                            << "endtime is:"
                            << END_TIME);

    NS_LOG_INFO("finish");
}
