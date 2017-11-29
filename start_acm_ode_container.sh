#!/bin/sh
echo ${DOCKER_SHARED_VOLUME}
echo ${DOCKER_HOST_IP}

USAGE="start_acm_ode_container.sh [CONFIG] [TYPE]"

if [ -z $1 ] || [ ! -f $1 ]; then
    echo "Config file: "$1" not found!"
    echo $USAGE
    exit 1
fi

if [ -z $2 ]; then
    echo "Must include type (encode or decode)!"
    echo $USAGE
    exit 1
elif [ $2 = "encode" ]; then
    TYPE=$2
elif [ $2 = "decode" ]; then
    TYPE=$2
else 
    echo "Must include type (encode or decode)!"
    echo $USAGE
    exit 1
fi

mkdir -p ${DOCKER_SHARED_VOLUME}

# Copy the config to the test data.
cp $2 ${DOCKER_SHARED_VOLUME}/config.properties

docker run -it -v ${DOCKER_SHARED_VOLUME}:/asn1-codec-data -e DOCKER_HOST_IP=${DOCKER_HOST_IP} asn1codec_asn1_codec:latest /asn1-codec/docker-test/acm_standalone.sh $TYPE
