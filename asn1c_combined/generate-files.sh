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

# If 2024, apply .ASN file edits
if [ "$year" == "2024" ]; then
    echo "Applying J2735 ASN Edits"
    patch --binary --backup --forward --reject-file="-" \
        ./j2735-asn-files/2024/J2945-3-RoadWeatherMessage-2024-rel-v2.1.asn \
        ./j2735-asn-files/2024/asn-edits/RoadWeatherMessage.patch
    patch --binary --backup --forward --reject-file="-" \
        ./j2735-asn-files/2024/J3217-R-RoadUserChargingReportMsg-2024-rel-v1.1.asn \
        ./j2735-asn-files/2024/asn-edits/RoadUserChargingReportMessage.patch
    patch --binary --backup --forward --reject-file="-" \
        ./j2735-asn-files/2024/J3217-TollUsageMsg-2024-rel-v1.1.asn \
        ./j2735-asn-files/2024/asn-edits/TollUsageMessage.patch

    # Verify that the patches were applied correctly
    if ! grep -q RwmSnapShot ./j2735-asn-files/2024/J2945-3-RoadWeatherMessage-2024-rel-v2.1.asn; then
        echo "The patch for the Road Weather Message ASN file was not applied correctly."
        exit 1
    fi
    
    if ! grep -q TumVehicleId ./j2735-asn-files/2024/J3217-R-RoadUserChargingReportMsg-2024-rel-v1.1.asn; then
        echo "The patch for the Road User Charging Report Message ASN file was not applied correctly."
        exit 1
    fi

    if ! grep -q TumVehicleId ./j2735-asn-files/2024/J3217-TollUsageMsg-2024-rel-v1.1.asn; then
        echo "The patch for the Toll Usage Message ASN file was not applied correctly."
        exit 1
    fi

fi


asn1c -fcompound-names -gen-OER -fincludes-quoted -no-gen-JER -pdu=all \
    ./scms-asn-files/*.asn \
    ./j2735-asn-files/$year/*.asn \
    ./semi-asn-files/$year/SEMI*.asn \
    -D ./generated-files/$year \
    2>&1 | tee compile.out

   

# if 2020 or 2024, copy overrides
if [ "$year" == "2020" ] || [ "$year" = "2024" ]; then
    echo "Copying overrides for $year"
    cp ./j2735-asn-files/$year/overrides/*.h ./generated-files/$year
    cp ./j2735-asn-files/$year/overrides/*.c ./generated-files/$year
fi


# tar generated files and delete originals
echo "Tarring generated files for $year"
tar -czf ./generated-files/$year.tar.gz ./generated-files/$year/*
rm -rf ./generated-files/$year