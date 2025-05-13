#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the ACM
if [ "${ACM_HTTP_SERVER}" == "true" ]; then
  echo "Starting HTTP Server"
  /build/acm -c /asn1_codec/config/${ACM_CONFIG_FILE} -v ${ACM_LOG_LEVEL} -R -H
else
  echo "Starting Kafka"
  /build/acm -c /asn1_codec/config/${ACM_CONFIG_FILE} -b ${DOCKER_HOST_IP}:9092 -v ${ACM_LOG_LEVEL} -R
fi

