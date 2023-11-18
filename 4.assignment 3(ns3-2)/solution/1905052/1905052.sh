#!/bin/bash

rm -fr ./scratch/Task1/TP_vs_BDT
mkdir -p ./scratch/Task1/TP_vs_BDT

rm -fr ./scratch/Task1/Bottle_Neck_Datarate
mkdir -p ./scratch/Task1/Bottle_Neck_Datarate

for i in $(seq 0 50 300)
do
    ./ns3 run "scratch/1905052.cc --bottlelinkrate=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task1/Bottle_Neck_Datarate/Throughput_Vs_Bottleneck_Data_Rate.png"
plot "scratch/Task1/TP_vs_BDT/data1.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task1/TP_vs_BDT/data2.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF

rm -fr ./scratch/Task1/TP_vs_PLR
mkdir -p ./scratch/Task1/TP_vs_PLR

rm -fr ./scratch/Task1/Packet_Loss_Rate
mkdir -p ./scratch/Task1/Packet_Loss_Rate

for i in $(seq 2 1 6)
do
    ./ns3 run "scratch/1905052.cc --power=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task1/Packet_Loss_Rate/Throughput_Vs_Packet_Loss_Rate.png"
plot "scratch/Task1/TP_vs_PLR/data3.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task1/TP_vs_PLR/data4.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF

rm -fr ./scratch/Task1/CW_vs_Time
mkdir -p ./scratch/Task1/CW_vs_Time

rm -fr ./scratch/Task1/Congestion_Window
mkdir -p ./scratch/Task1/Congestion_Window

./ns3 run "scratch/1905052.cc"

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task1/Congestion_Window/Congestion_Window_VS_Time.png"
plot "scratch/Task1/CW_vs_Time/flow1.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task1/CW_vs_Time/flow2.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF




ca1="ns3::TcpNewReno"
ca2="ns3::TcpAdaptiveReno"

rm -fr ./scratch/Task2/TP_vs_BDT
mkdir -p ./scratch/Task2/TP_vs_BDT

rm -fr ./scratch/Task2/Bottle_Neck_Datarate_1
mkdir -p ./scratch/Task2/Bottle_Neck_Datarate_1

for i in $(seq 0 50 300)
do
    ./ns3 run "scratch/1905052.cc --bottlelinkrate=${i} --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Bottle_Neck_Datarate_1/Throughput_Vs_Bottleneck_Data_Rate.png"
plot "scratch/Task2/TP_vs_BDT/data1.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task2/TP_vs_BDT/data2.txt" using 1:2 title 'adaptivereno' with linespoints
exit
EOF


rm -fr ./scratch/Task2/TP_vs_PLR
mkdir -p ./scratch/Task2/TP_vs_PLR

rm -fr ./scratch/Task2/Packet_Loss_Rate_1
mkdir -p ./scratch/Task2/Packet_Loss_Rate_1

for i in $(seq 2 1 6)
do
    ./ns3 run "scratch/1905052.cc --power=${i} --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Packet_Loss_Rate_1/Throughput_Vs_Packet_Loss_Rate.png"
plot "scratch/Task2/TP_vs_PLR/data3.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task2/TP_vs_PLR/data4.txt" using 1:2 title 'adaptivereno' with linespoints
exit
EOF


rm -fr ./scratch/Task2/CW_vs_Time
mkdir -p ./scratch/Task2/CW_vs_Time

rm -fr ./scratch/Task2/Congestion_Window_1
mkdir -p ./scratch/Task2/Congestion_Window_1

./ns3 run "scratch/1905052.cc --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Congestion_Window_1/Congestion_Window_VS_Time.png"
plot "scratch/Task2/CW_vs_Time/flow1.txt" using 1:2 title 'newreno' with linespoints, \
     "scratch/Task2/CW_vs_Time/flow2.txt" using 1:2 title 'adaptivereno' with linespoints
exit
EOF




ca1="ns3::TcpAdaptiveReno"
ca2="ns3::TcpHighSpeed"


rm -fr ./scratch/Task2/TP_vs_BDT
mkdir -p ./scratch/Task2/TP_vs_BDT

rm -fr ./scratch/Task2/Bottle_Neck_Datarate_2
mkdir -p ./scratch/Task2/Bottle_Neck_Datarate_2


for i in $(seq 0 50 300)
do
    ./ns3 run "scratch/1905052.cc --bottlelinkrate=${i} --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Bottle_Neck_Datarate_2/Throughput_Vs_Bottleneck_Data_Rate.png"
plot "scratch/Task2/TP_vs_BDT/data1.txt" using 1:2 title 'adaptivereno' with linespoints, \
     "scratch/Task2/TP_vs_BDT/data2.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF


rm -fr ./scratch/Task2/TP_vs_PLR
mkdir -p ./scratch/Task2/TP_vs_PLR

rm -fr ./scratch/Task2/Packet_Loss_Rate_2
mkdir -p ./scratch/Task2/Packet_Loss_Rate_2

for i in $(seq 2 1 6)
do
    ./ns3 run "scratch/1905052.cc --power=${i} --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Packet_Loss_Rate_2/Throughput_Vs_Packet_Loss_Rate.png"
plot "scratch/Task2/TP_vs_PLR/data3.txt" using 1:2 title 'adaptivereno' with linespoints, \
     "scratch/Task2/TP_vs_PLR/data4.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF


rm -fr ./scratch/Task2/CW_vs_Time
mkdir -p ./scratch/Task2/CW_vs_Time

rm -fr ./scratch/Task2/Congestion_Window_2
mkdir -p ./scratch/Task2/Congestion_Window_2

./ns3 run "scratch/1905052.cc --tcpVariant1=${ca1} --tcpVariant2=${ca2} --task=2"

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Task2/Congestion_Window_2/Congestion_Window_VS_Time.png"
plot "scratch/Task2/CW_vs_Time/flow1.txt" using 1:2 title 'adaptivereno' with linespoints, \
     "scratch/Task2/CW_vs_Time/flow2.txt" using 1:2 title 'tcphs' with linespoints
exit
EOF