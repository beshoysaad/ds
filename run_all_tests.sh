#!/bin/bash
./start.sh
./test-lab-4-a.pl ./yfs1
./test-lab-4-b ./yfs1 ./yfs2
./test-lab-4-b ./yfs1 ./yfs1
./test-lab-4-c ./yfs1 ./yfs2
./test-lab-4-c ./yfs1 ./yfs1
./stop.sh

export RPC_LOSSY=5
./start.sh
./test-lab-4-a.pl ./yfs1
./test-lab-4-b ./yfs1 ./yfs2
./test-lab-4-b ./yfs1 ./yfs1
./test-lab-4-c ./yfs1 ./yfs2
./test-lab-4-c ./yfs1 ./yfs1
./stop.sh
