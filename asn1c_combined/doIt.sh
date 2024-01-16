#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2020
if [ -z "$J2735_YEAR" ]; then
    year="2020"
else
    year=$J2735_YEAR
fi

asn1c -fcompound-names -gen-OER -fincludes-quoted -no-gen-JER -pdu=all \
    ../scms-asn1/1609dot2-asn/1609dot2-base-types.asn \
    ../scms-asn1/1609dot2-asn/1609dot2-schema.asn \
    ./j2735-asn-files/$year/*.asn \
    ./SEMI_v2.4.0_121223_j2020.asn \
    2>&1 | tee compile.out

# if 2020, copy overrides
if [ "$year" == "2020" ]; then
    echo "Copying overrides for $year"
    cp ./j2735-asn-files/$year/overrides/*.h .
fi

sed -i 's/\(-DASN_PDU_COLLECTION\)/-DPDU=MessageFrame \1/' converter-example.mk

make -f converter-example.mk
