#!/bin/bash

rm -fr ./scratch/graphs1
mkdir -p ./scratch/graphs1

rm -fr ./scratch/Static
mkdir -p ./scratch/Static

for i in $(seq 20 20 100)
do
    ./ns3 run "scratch/1905052_1.cc --total_node=${i} --total_flow=${i/2}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_nodes_throughput.png"
plot "scratch/graphs1/data1.txt" using 1:2 title 'Throughput vs Nodes' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_nodes_deliveryRatio.png"
plot "scratch/graphs1/data2.txt" using 1:2 title 'Delivary Ratio vs Nodes' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_flows_throughput.png"
plot "scratch/graphs1/data3.txt" using 1:2 title 'Throughput vs Flows' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_flows_deliveryRatio.png"
plot "scratch/graphs1/data4.txt" using 1:2 title 'Delivary Ratio vs Flows' with linespoints
exit
EOF


rm -fr ./scratch/graphs1
mkdir -p ./scratch/graphs1

for i in $(seq 100 100 500)
do
    ./ns3 run "scratch/1905052_1.cc --packetPersecond=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_packetpersecond_throughput.png"
plot "scratch/graphs1/data5.txt" using 1:2 title 'Throughput vs PacketPersecond' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_packetpersecond_delivaryRatio.png"
plot "scratch/graphs1/data6.txt" using 1:2 title 'Delivary Ratio vs PacketPersecond' with linespoints
exit
EOF


rm -fr ./scratch/graphs1
mkdir -p ./scratch/graphs1

for i in $(seq 1 1 5)
do
    ./ns3 run "scratch/1905052_1.cc --coverage_area=${i}"
done

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_coverageArea_throughput.png"
plot "scratch/graphs1/data7.txt" using 1:2 title 'Throughput vs Coverage Area' with linespoints
exit
EOF

gnuplot << EOF
set terminal png size 640,480
set output "scratch/Static/static_coverageArea_delivaryRatio.png"
plot "scratch/graphs1/data8.txt" using 1:2 title 'Delivary Ratio vs Coverage Area' with linespoints
exit
EOF