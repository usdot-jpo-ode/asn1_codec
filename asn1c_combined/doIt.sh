#!/bin/bash

# This script assumes that the J2735 files have already been generated.
# To generate the J2735 files, run generate-files.sh with asn1c installed.

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2020
if [ -z "$J2735_YEAR" ]; then
    year="2020"
else
    year=$J2735_YEAR
fi

# Copy generated files to for specified year to asn1c_combined & extract
echo "Extracting & copying generated files for $year"
tar -xzf ./generated-files/$year.tar.gz
cp ./generated-files/$year/* .

# Compile example
echo "Compiling example"
sed -i 's/\(-DASN_PDU_COLLECTION\)/-DPDU=MessageFrame \1/' ./converter-example.mk
make -f ./converter-example.mk

# Clean up
echo "Cleaning up"
rm -rf ./generated-files/$year