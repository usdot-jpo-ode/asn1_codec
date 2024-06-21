#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib

echo ""

DOCKER_HOST_IP=$2

# if DOCKER_HOST_IP is not set, warn and exit
if [ -z $DOCKER_HOST_IP ]
then
    echo "DOCKER_HOST_IP is not set. Exiting."
    exit 1
fi

echo "**************************"
echo "Producing Data ..."
echo "**************************"
# Produce data with test_in.py and pipe it to kafka_tool, which sends the data to the topic.
cat /asn1_codec_data/test.data | /asn1_codec/docker-test/test_in.py | /build/kafka-test/kafka_tool -P -b $DOCKER_HOST_IP:9092 -p 0 -t topic.Asn1EncoderInput 2> prod.err
if [ $? -ne 0 ]; then
    cat prod.err
    echo "Failed to produce data. Exiting."
    exit 1
fi

# Start the DI consumer.
offset=$1

echo "**************************"
echo "Consuming Data at offset "$offset "from topic.Asn1EncoderOutput targeting "$DOCKER_HOST_IP" ..." 
echo "**************************"

while true; do
    # Consume data from the topic with kafka_tool and pipe it to test_out.py, then write the output to tmp.out
    /build/kafka-test/kafka_tool -C -b $DOCKER_HOST_IP:9092 -p 0 -t topic.Asn1EncoderOutput -e -o $offset 2> con.err | /asn1_codec/docker-test/test_out.py > tmp.out
    if [ $? -ne 0 ]; then
        cat con.err
        echo "Failed to consume data. Exiting."
        exit 1
    fi

    # Check if the output is not empty
    lines=$(cat tmp.out | wc -l)

    # If the output is not empty, print the output and break the loop
    if [[ $lines != "0" ]]; then 
        cat tmp.out

        break
    fi

    # If the output is empty, print the error message and retry
    cat con.err
done

echo ""