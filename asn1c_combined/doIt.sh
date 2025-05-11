#!/bin/bash

# This script assumes that the J2735 files have already been generated.
# To generate the J2735 files, run generate-files.sh with asn1c installed.

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2024
if [ -z "$J2735_YEAR" ]; then
    year="2024"
else
    year=$J2735_YEAR
fi

# Copy generated files to for specified year to asn1c_combined & extract
echo "Extracting & copying generated files for $year"
tar -xzf ./generated-files/$year.tar.gz
cp ./generated-files/$year/* .

# Compile example
echo "Compiling example"
# The HAVE_TM_GMTOFF flag needs to be set to compile GeneralizedTime.c on platforms including Alpine.
# Add it to ASN_MODULE_CFLAGS which gets added to CFLAGS in converter-example.mk
make ASN_MODULE_CFLAGS="-DHAVE_TM_GMTOFF" -f ./converter-example.mk
# Verify that the converter example executable compiled and can run
./converter-example -p list


# Clean up
echo "Cleaning up"
rm -rf ./generated-files/$year