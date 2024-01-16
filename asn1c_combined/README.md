# doIt.sh Script
The `doIt.sh` script will generate the C code for the J2735 ASN.1 files.

## J2735 ASN Files
The 'j2735-asn-files' subdirectory should contain the ASN.1 files for the J2735 standard. These are organized by year.

The `doIt.sh` script will reference the necessary files from the `j2735-asn-files` directory when generating the C code.

### Obtaining the J2735 ASN Files
Redistribution of the ASN files is not permitted, so they are not included in this repository. You must obtain them from SAE and place them in the `j2735-asn-files` directory under the appropriate year subdirectory.

A table of the ASN files for each year is provided below.

| Year | Source |
| ---- | ----------- |
| 2016 | https://www.sae.org/standards/content/j2735set_201603/ |
| 2020 | https://www.sae.org/standards/content/j2735set_202007/ |

## Environment Variables
The `doIt.sh` script uses the following environment variables:
- `J2735_YEAR` - The year of the J2735 standard to use. For example, `2020`.

# Troubleshooting
## Fix for Makefile

The following compile flags must be set for the converter-example.c to compile (whether you need this or not is another story).

- -DPDU=<something>     // this allows the #ifndef statements in the above c file to work correctly.
- -DASN_PDU_COLLECTION  // if you have several message types you want to use.
