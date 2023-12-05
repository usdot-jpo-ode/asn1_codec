# J2735 ASN Files
The 'j2735-asn-files' subdirectory contains the ASN.1 files for the J2735 standard. These are organized by year. For example, you can find the 2016 files in the 'j2735-asn-files 2016' directory.

The `doIt.sh` script will reference the necessary files from the `j2735-asn-files` directory.

# Fixes
## Fix for name mangling problems

## Fix for Makefile

The following compile flags must be set for the converter-example.c to compile (whether you need this or not is another story).

- -DPDU=<something>     // this allows the #ifndef statements in the above c file to work correctly.
- -DASN_PDU_COLLECTION  // if you have several message types you want to use.


