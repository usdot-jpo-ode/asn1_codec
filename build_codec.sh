#!/bin/sh
# Build and install asn1c submodule
cd /asn1_codec/asn1c
echo configuring asn1c compiler ...
test -f configure || autoreconf -iv && ./configure

echo Building asn1c libraries and the compiler ...
make

# Ensure asn1c is still behaving well after compiling on your platform
echo Testing asn1c compiler ...
make check

# Use ./configure --prefix to override install location.
echo Installing asn1c compiler standard location ...
make install

# configure OS
export LD_LIBRARY_PATH=/usr/local/lib CC=gcc

echo Generating SAE J2735 API ...
mkdir -p /asn1_codec/sae_j2735_SEMI_api && cd /asn1_codec/sae_j2735_SEMI_api
make

echo Generating IEEE 1609.2 API
mkdir -p /asn1_codec/ieee_1609dot2_api && cd /asn1_codec/ieee_1609dot2_api
make

#echo building asn1_codec ...
#mkdir -p /asn1_codec/build && cd /asn1_codec/build
#cmake /asn1_codec
#make

