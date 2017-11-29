#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

# Start the DI tool.
/build/acm -c /asn1_codec_data/config.properties -T $1 -o end
