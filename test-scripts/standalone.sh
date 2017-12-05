#!/bin/bash

# There are two input files: CONFIG, TEST_DATA.
# Offset is the offset in the topic that will be consumed and displayed in the
# output
# Type is the type of operation (encode or decode)
USAGE="standalone.sh [CONFIG] [TEST_FILE] [TYPE] [OFFSET]"

if [ -z $1 ] || [ ! -f $1 ]; then
    echo "Config file: "$1" not found!"
    echo $USAGE
    exit 1
fi

if [ -z $2 ] || [ ! -f $2 ]; then
    echo "Test file: "$2" not found!"
    echo $USAGE
    exit 1
fi

if [ -z $3 ]; then
    echo "Must include type (encode or decode)!"
    echo $USAGE
    exit 1
elif [ $3 = "encode" ]; then
    TYPE=$3
elif [ $3 = "decode" ]; then
    TYPE=$3
else 
    echo "Must include type (encode or decode)!"
    echo $USAGE
    exit 1
fi

if [ -z $4 ]; then
    OFFSET=0
else
    OFFSET=$4
fi

mkdir -p /tmp/docker-test/asn1-codec/data

# Copy the config to the test data.
# TODO replace map file line: sed -i '/TEXT_TO_BE_REPLACED/c\This line is removed by the admin.' /tmp/foo
cp $1 /tmp/docker-test/asn1-codec/data/config.properties

# Copy the data.
cp $2 /tmp/docker-test/asn1-codec/data/test.data

echo "**************************"
echo "Running standalone test with "$1 $2 $3 $4
echo "**************************"

# Start the PPM in a new container.
docker run --name acm_kafka -v /tmp/docker-test/asn1-codec/data:/asn1_codec_data -it --rm -p '8080:8080' -d asn1codec_asn1_codec:latest /asn1_codec/docker-test/acm_standalone.sh $TYPE > /dev/null

sleep 10

# Produce the test data.
docker exec acm_kafka /asn1_codec/docker-test/do_test.sh $OFFSET
docker stop acm_kafka > /dev/null
