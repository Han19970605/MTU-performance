#!/bin/bash
#test save
loss_rate=100
FileName="TcpNewReno_100.csv"
protocol="ns3::TcpNewReno"

for delay in '5us' '2ms' '50ms'
do
    for bandwidth in '16Mbps' '128Mbps' '1Gbps'
    do
    for mtu in 400 800 1000 1200 1500 1800 2000 2200 2500 2800 3000 3300 3500 3800 4000 4500 5000 5500 6000 7000 8000 9000
    do
        ./waf --run "test_new_netdevice --MTU=$mtu --LOSS_RATE=$loss_rate --TCP_PROTOCOL=$protocol --DATA_RATE=$bandwidth --DELAY=$delay --FileName=$FileName"
    done 
done
done
