#!/bin/sh

# Helpers
GREEN='\033[92m'
RED='\033[91m'
NC='\033[0m'
CYAN='\033[96m'
TICK="[${GREEN}✓${NC}]"
CROSS="[${RED}✗${NC}]"
INFO="[${CYAN}i${NC}]"

initializeSubmodules(){
    # initialize submodules
    echo "${GREEN}Initializing submodules${NC}"
    git submodule update --recursive --init
    git pull --recurse-submodules
}

buildAsn1c(){
    # build asn1c
    echo "${GREEN}Building asn1c${NC}"
    cd ./asn1c
    git reset --hard
    git pull origin master
    aclocal
    test -f ./configure || autoreconf -iv
    ./configure
    make
    sudo make install 
    # get back to main directory
    cd ../
}

buildPugiXml(){
    # build pugixml
    echo "${GREEN}Building pugixml${NC}"
    cd ./pugixml
    git pull origin master
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    # get back to main directory
    cd ../..
}

compileSpecAndBuildLibrary(){
    # Compile the Specifications and Build the Library
    echo "${GREEN}Compile spec and build library${NC}"
    cd ./asn1c_combined
    ./doIt.sh
    # get back to main directory
    cd ../
}

buildACM(){
    # Build the ACM
    echo "${GREEN}Building ACM${NC}"
    rm -r build
    mkdir build
    cd build
    cmake ..
    make
    # get back to main directory
    cd ../
}

runTests() {
    # assumes everything has been built
    cd ./build
    ./acm_tests
    cd ../
}

runAll(){
    # following from instructions in installation.md, this is the proper build order
    initializeSubmodules
    buildAsn1c
    buildPugiXml
    compileSpecAndBuildLibrary
    buildACM
    runTests
}

help(){
    echo "Usage: ./build_local.sh [option]"
    echo "Options:"
    echo "${CYAN}  -h, --help${NC}: Print this help message"
    echo "${CYAN}  -a, --asn1c${NC}: Build asn1c"
    echo "${CYAN}  -p, --pugi${NC}: Build pugi"
    echo "${CYAN}  -c, --compile${NC}: Compile the specifications and build the library"
    echo "${CYAN}  -b, --build${NC}: Build the ACM"
    echo "${CYAN}  -t, --test${NC}: Execute tests"
    echo "${CYAN}  -r, --run${NC}: Run all"
}


case "${1}" in
    -h|--help) help ;;
    -a|--asn1c) buildAsn1c ;;
    -p|--pugi) buildPugiXml ;;
    -c|--compile) compileSpecAndBuildLibrary ;;
    -b|--build) buildACM ;;
    -t|--test) runTests ;;
    -r|--run) runAll ;;
    *) runAll ;;
esac