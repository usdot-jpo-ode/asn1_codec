#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

asn1c -fcompound-names -gen-PER -gen-OER -pdu=all \
    ../scms-asn1/1609dot2-asn/1609dot2-base-types.asn \
    ../scms-asn1/1609dot2-asn/1609dot2-schema.asn \
    J2735_201603DA.ASN \
    SEMI_v2.3.0_070616.asn \
    2>&1 | tee compile.out

sed -i 's/\(-DASN_PDU_COLLECTION\)/-DPDU=MessageFrame \1/' converter-example.mk

make -f converter-example.mk
