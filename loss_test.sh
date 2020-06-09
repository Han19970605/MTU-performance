#!/bin/bash
#test save
nohup ./TcpNewReno_10.sh >./log/Tcpnewreno_10.txt &
nohup ./TcpNewReno_100.sh >./log/TcpNewReno_100.txt &
nohup ./TcpBbr_10.sh >./log/TcpBbr_10.txt &
nohup ./TcpBbr_100.sh >./log/TcpBbr_100.txt &
nohup ./TcpVegas_10.sh >./log/TcpVegas_10.txt &
nohup ./TcpVegas_100.sh >./log/TcpVegas_100.txt &
