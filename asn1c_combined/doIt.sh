#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2016
if [ -z "$J2735_YEAR" ]; then
    year="2016"
else
    year=$J2735_YEAR
fi

asn1c -fcompound-names -gen-PER -gen-OER -pdu=all \
    ../scms-asn1/1609dot2-asn/1609dot2-base-types.asn \
    ../scms-asn1/1609dot2-asn/1609dot2-schema.asn \
    ./j2735-asn-files/$year/*.asn \
    SEMI_v2.3.0_070616.asn \
    2>&1 | tee compile.out

sed -i 's/\(-DASN_PDU_COLLECTION\)/-DPDU=MessageFrame \1/' converter-example.mk

make -f converter-example.mk
