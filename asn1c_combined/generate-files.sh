#!/bin/bash

# This script generates the J2735 files for a given year.
# The generated files are included in source control, so this script should only
# need to be run when the J2735 files need to be updated.

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2024
if [ -z "$J2735_YEAR" ]; then
    year="2024"
else
    year=$J2735_YEAR
fi

# if generated-files directory does not exist, create it
if [ ! -d "./generated-files" ]; then
    mkdir ./generated-files
fi

# if generated-files/year directory does not exist, create it
if [ ! -d "./generated-files/$year" ]; then
    mkdir ./generated-files/$year
fi

asn1c -fno-include-deps -fcompound-names -fcase-insensitive-filenames -gen-OER -fincludes-quoted -no-gen-JER -pdu=all \
    ./scms-asn-files/*.asn \
    ./j2735-asn-files/$year/*.asn \
    ./semi-asn-files/$year/SEMI*.asn \
    -D ./generated-files/$year \
    2>&1 | tee compile.out


# tar generated files and delete originals
echo "Tarring generated files for $year"
tar -czf ./generated-files/$year.tar.gz ./generated-files/$year/*
rm -rf ./generated-files/$year