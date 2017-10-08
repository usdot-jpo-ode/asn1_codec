#!/bin/bash

# asn1c -Wdebug-compiler -fcompound-names -gen-PER -pdu=all 1609dot2-base-types.asn 1609dot2-schema.asn J2735_201603DA.ASN 2>&1 | tee compile.out
asn1c -fcompound-names -gen-PER -pdu=all 1609dot2-base-types.asn 1609dot2-schema.asn J2735_201603DA.ASN 2>&1 | tee compile.out

sed -i 's/\(-DASN_PDU_COLLECTION\)/-DPDU=MessageFrame \1/' Makefile.am.example

sed -i 's/#include "Latitude.h"/#include "DSRC_Latitude.h"/' *DLocation.h
sed -i 's/#include "Longitude.h"/#include "DSRC_Longitude.h"/' *DLocation.h
sed -i 's/#include "Elevation.h"/#include "DSRC_Elevation.h"/' *DLocation.h

sed -i 's/Latitude_t/DSRC_Latitude_t/' *DLocation.h
sed -i 's/Longitude_t/DSRC_Longitude_t/' *DLocation.h
sed -i 's/Elevation_t/DSRC_Elevation_t/' *DLocation.h

sed -i 's/\&asn_DEF_Latitude/\&asn_DEF_DSRC_Latitude/' *DLocation.c
sed -i 's/\&asn_DEF_Elevation/\&asn_DEF_DSRC_Elevation/' *DLocation.c
sed -i 's/\&asn_DEF_Longitude/\&asn_DEF_DSRC_Longitude/' *DLocation.c

make -f Makefile.am.example
