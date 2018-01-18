#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the ACM
/build/acm -c /asn1_codec/config/${ACM_CONFIG_FILE} -b ${DOCKER_HOST_IP}:9092 -R
