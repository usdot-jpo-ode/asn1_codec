# Fix for name mangling problems

The following files should be checked (can't remember if some of these are OK)

- TwoDLocation.h
    - #include "Latitude.h"
    - #include "Longitude.h"
    - Latitude_t
    - Longitude_t

- ThreeDLocation.h
    - #include "Latitude.h"
    - #include "Longitude.h"
    - #include "Elevation.h"
    - Latitude_t
    - Longitude_t
    - Elevation_t

- ThreeDLocation.c
    - &asn_DEF_Latitude -> &asn_DEF_DSRC_Latitude
    - &asn_DEF_Longitude -> &asn_DEF_DSRC_Longitude
    - &asn_DEF_Elevation -> &asn_DEF_DSRC_Elevation

- TwoDLocation.c
    - &asn_DEF_Latitude -> &asn_DEF_DSRC_Latitude
    - &asn_DEF_Longitude -> &asn_DEF_DSRC_Longitude
    - &asn_DEF_Elevation -> &asn_DEF_DSRC_Elevation

The following names should be updated:

- Latitude.h -> DSRC_Latitude.h
- Longitude.h -> DSRC_Longitude.h
- Elevation.h -> DSRC_Elevation.h
- Latitude_t -> DSRC_Latitude_t
- Longitude_t -> DSRC_Longitude_t
- Elevation_t -> DSRC_Elevation_t
- asn_DEF_Latitude -> asn_DEF_DSRC_Latitude
- asn_DEF_Longitude -> asn_DEF_DSRC_Longitude
- asn_DEF_Elevation -> asn_DEF_DSRC_Elevation

# Fix for Makefile

The following compile flags must be set for the converter-example.c to compile (whether you need this or not is another
story).

- -DPDU=<something>     // this allows the #ifndef statements in the above c file to work correctly.
- -DASN_PDU_COLLECTION  // if you have several message types you want to use.


