#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib
mkdir -p /asn1_codec/build && cd /asn1_codec/build
cmake /asn1_codec
make
