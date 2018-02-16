#!/bin/bash

grep -hr -A 14 -E "asn_TYPE_descriptor_t +asn_DEF_.+ += +{" --include=*.{h,c} "${1}" | python dynamic_types_parse.py 


