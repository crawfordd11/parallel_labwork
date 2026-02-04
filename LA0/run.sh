#!/bin/bash

GREEN='\033[1;32m'
NC='\033[0m'
YELLOW='\033[1;33m'
echo
echo -e ${GREEN}Compiling Test Suite...${NC}
temp=$(make clean)
make all
echo
echo -e ${GREEN}Submitting Test Suite to Cluster...${NC}
output=$(sbatch sub_LA0)
echo $output
#num="$(cut -d'.' -f1 <<<"$output")"
num=$(echo "$output" | awk '{print $NF}')
file=test_LA0.$num.out
while ! test -f "./$file"
do
    sleep 2
done
#temp=$(make clean)
echo
echo -e ${GREEN}Open ${YELLOW}test_LA0.$num.out${GREEN} to see Test Results${NC}
echo
