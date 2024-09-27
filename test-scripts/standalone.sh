#!/bin/bash

# This script runs the standalone test for the ACM. It starts the ACM in a new container, produces
# and consumes the test data with `do_test.sh`, and then stops the ACM container. Finally, it
# reports the result of the test back to the caller.

# This script is used by `do_kafka_test.sh`

ACM_CONTAINER_NAME=test_acm_instance
ACM_IMAGE_TAG=do-kafka-test-acm-image
ACM_IMAGE_NAME=asn1_codec-acm

startACMContainer() {
    # make sure ip can be pinged
    while true; do
        if ping -c 1 $DOCKER_HOST_IP &> /dev/null; then
            break
        else
            echo "Docker host ip $DOCKER_HOST_IP is not pingable. Exiting."
            exit 1
        fi
    done
    echo "Starting ACM in new container '$ACM_CONTAINER_NAME'"
    docker run --name $ACM_CONTAINER_NAME --env ACM_LOG_TO_CONSOLE=true --env ACM_LOG_LEVEL=DEBUG -v /tmp/docker-test/asn1-codec/data:/asn1_codec_data -it --rm -p '8080:8080' -d $ACM_IMAGE_NAME:$ACM_IMAGE_TAG /asn1_codec/docker-test/acm_standalone.sh $TYPE

    echo "Waiting for $ACM_CONTAINER_NAME to spin up"
    # while num lines of docker logs is less than 100, sleep 1
    secondsToWait=10
    sleep $secondsToWait # TODO: use check like log count to determine when to stop waiting
    echo "$ACM_CONTAINER_NAME is ready after $secondsToWait seconds"

    if [ $(docker ps | grep $ACM_CONTAINER_NAME | wc -l) == "0" ]; then
        echo "ACM container '$ACM_CONTAINER_NAME' is not running. Exiting."
        exit 1
    fi
}

stopACMContainer() {
    echo "Stopping and removing existing container if it exists"
    docker stop $ACM_CONTAINER_NAME > /dev/null
    docker rm $ACM_CONTAINER_NAME > /dev/null
}

# There are two input files: CONFIG, TEST_DATA.
# Offset is the offset in the topic that will be consumed and displayed in the output
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
cp $1 /tmp/docker-test/asn1-codec/data/config.properties

# identify operating system
OS=$(uname)
if [ $OS = "Darwin" ]; then
    # change metadata.broker.list to value of DOCKER_HOST_IP
    sed -i '' "s/metadata.broker.list=.*/metadata.broker.list=$DOCKER_HOST_IP:9092/" /tmp/docker-test/asn1-codec/data/config.properties
    # change acm.type to encode or decode depending on the type
    sed -i '' "s/acm.type=.*/acm.type=$TYPE/" /tmp/docker-test/asn1-codec/data/config.properties
else
    # change metadata.broker.list to value of DOCKER_HOST_IP
    sed -i "s/metadata.broker.list=.*/metadata.broker.list=$DOCKER_HOST_IP:9092/" /tmp/docker-test/asn1-codec/data/config.properties
    # change acm.type to encode or decode depending on the type
    sed -i "s/acm.type=.*/acm.type=$TYPE/" /tmp/docker-test/asn1-codec/data/config.properties
fi

# Copy the data.
cp $2 /tmp/docker-test/asn1-codec/data/test.data

echo "**************************"
echo "Running standalone test with "$1 $2 $3 $4
echo "**************************"

# Stop and remove existing container if it exists
stopACMContainer

# Start the ACM in a new container.
startACMContainer

# Produce and consume the test data.
docker exec $ACM_CONTAINER_NAME /asn1_codec/docker-test/do_test.sh $OFFSET $DOCKER_HOST_IP

# return 1 if the test fails
if [ $? -ne 0 ]; then
    docker stop $ACM_CONTAINER_NAME > /dev/null
    exit 1
fi

docker stop $ACM_CONTAINER_NAME > /dev/null

echo ""
