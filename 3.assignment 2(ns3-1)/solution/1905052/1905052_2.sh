#!/bin/bash

rm -fr ./scratch/graphs2
mkdir -p ./scratch/graphs2

rm -fr ./scratch/Mobile
mkdir -p ./scratch/Mobile

for i in $(seq 20 20 100)
do
    ./ns3 run "scratch/1905052_2.cc --total_node=${i} --total_flow=${i/2}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/nodes_throughput.png"
plot "scratch/graphs2/data1.txt" using 1:2 title 'Throughput vs Nodes' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/nodes_deliveryRatio.png"
plot "scratch/graphs2/data2.txt" using 1:2 title 'Delivary Ratio vs Nodes' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/flows_throughput.png"
plot "scratch/graphs2/data3.txt" using 1:2 title 'Throughput vs Flows' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/flows_deliveryRatio.png"
plot "scratch/graphs2/data4.txt" using 1:2 title 'Delivary Ratio vs Flows' with linespoints
exit
EOF


rm -fr ./scratch/graphs2
mkdir -p ./scratch/graphs2

for i in $(seq 100 100 500)
do
    ./ns3 run "scratch/1905052_2.cc --packetPersecond=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/packetpersecond_throughput.png"
plot "scratch/graphs2/data5.txt" using 1:2 title 'Throughput vs PacketPersecond' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/packetpersecond_delivaryRatio.png"
plot "scratch/graphs2/data6.txt" using 1:2 title 'Delivary Ratio vs PacketPersecond' with linespoints
exit
EOF


rm -fr ./scratch/graphs2
mkdir -p ./scratch/graphs2

for i in $(seq 5 5 25)
do
    ./ns3 run "scratch/1905052_2.cc --speed=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/speed_throughput.png"
plot "scratch/graphs2/data7.txt" using 1:2 title 'Throughput vs Speed' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Mobile/speed_delivaryRatio.png"
plot "scratch/graphs/data8.txt" using 1:2 title 'Delivary Ratio vs Speed' with linespoints
exit
EOF