#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the DI tool.
/build/acm -c /asn1_codec_data/config.properties -b ${DOCKER_HOST_IP}:9092 -T $1 -o end
