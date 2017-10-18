#!/bin/sh
# Build and install asn1c submodule
cd /home/asn1_codec/asn1c
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

echo Generating ASN.1 API ...
cd /home/asn1_codec/asn1c_combined
chmod +x ./doIt.sh && ./doIt.sh
cd ..
mkdir -p build && cd build
cmake .. && make

# build asn1_codec
cd /home/asn1_codec && mkdir -p build && cd build && cmake .. && make
