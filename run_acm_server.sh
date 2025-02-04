#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the ACM HTTP server
/build/acm -c /asn1_codec/config/${ACM_CONFIG_FILE} -v ${ACM_LOG_LEVEL} -R -H
