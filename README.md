# Abstract Syntax Notation One (ASN.1) Codec Module for the Operational Data Environment (ODE)

The ASN.1 Codec Module (ACM) processes Kafka data streams that preset 
[ODE Metadata]( wrapped ASN.1 da://github.com/usdot-jpo-ode/jpo-ode/blob/develop/docs/Metadata_v3.md).
It can perform one of two functions depending on how it is started:

1. **Decode**: This function is used to process messages *from* the connected
vehicle environment *to* ODE subscribers. Specifically, the ACM extacts binary
data from consumed messages (ODE Metatdata Messages) and decodes the binary
ASN.1 data into a structure that is subsequently encoded into an alternative
format more suitable for ODE subscribers (currently XML using XER).

1. **Encode**: This function is used to process messages *from* the ODE *to*
the connected vehicle environment. Specifically, the ACM extracts
human-readable data from ODE Metadata and decodes it into a structure that
is subsequently **encoded into ASN.1 binary data**.

![ASN.1 Codec Operations](docs/graphics/asn1codec-operations.png)

## Current ACM Data Types

- The ACM handles inbound (from CV environment to ODE) Basic Safety Messages (BSMs)
and Traveler Information Messages (TIMs) as SAE J2735 Message Frames. It will also
handle the Message Frames if wrapped in a IEEE 1609.2 Data frame.

	 - The IEEE 1609.2 Data Frame is usually encoded using COER rules, but the
       module will use metadata provided by the ODE to determine the decoder to use.

	 - The SAE J2735 Message Frame is usually encoded using UPER rules, but the
	   module will use metadata provided by the ODE to determine the decoder to use.
     
- The ACM handles outbound (from ODE to CV environment) TIMs as SAE J2735 Message Frames.

	 - Currently, the SAE J2735 Message Frame is encoded by the ODE using XER
       rules. The ACM encodes these using UPER rules to produce ASN.1 for transmission
       to the CV environment.

## Table of Contents

1. [Release Notes](#release-notes)
1. [Project Management](#project-management)
1. [Code Management and Deployment](#code-management-and-deployment)
1. [Downloading the Source Code and Documentation](#downloading-the-source-code-and-documentation)
1. [Documentation](#documentation)
1. [Code Documentation](#code-documentation)
1. [Installation](docs/installation.md)
1. [Configuration and Operation](docs/configuration.md)
1. [Testing](docs/testing.md)
1. [Development](docs/coding-standards.md)

## Release Notes

### ODE Sprint 24

- ODE-537/581/584/585/586/537: ASN.1 codec module development and integration. 

# Development and Collaboration Tools

## Project Management

This project is managed using the Jira tool.

- [Jira Project Portal](https://usdotjpoode.atlassian.net/secure/Dashboard.jsp)

## Code Management and Deployment

Detailed [installation instructions](docs/installation.md) are provided on another page, so we recommend starting there
if you want to run the ACM. If you just want to examine the code and documentation, you will need Git.

- [Git](https://git-scm.com/)

# Getting Started

## Downloading the Source Code and Documentation

Follow these instructions to download the source code and documentation onto your machine.

1. Clone the asn1_codec repository. Make sure to include the --recrusive option in `git clone` command line so that all required submodules (asn1c, pugixml and scms-asn) are also cloned.
```bash
git clone --recurse-submodules https://github.com/usdot-jpo-ode/asn1_codec.git
```
or

```bash
git clone --recurse-submodules git@github.com:usdot-jpo-ode/asn1_codec.git
```

1. Follow the instructions in the asn1c repository file INSTALL.md
1. [j2735 Specification must be purchased from SAE] Find your J2735 asn file and copy that into the asn1_codec repository.
1. The `doIt.sh` can perform the needed operations to compile the ASN specifications and fixup the Makefile so the CLI tool compiles. The individual commands that you need to run should you choose not to run `doIt.sh` are the following:
    1. Compile the ASN specifictions: `asn1c -fcompound-names -gen-PER -pdu=all ../scms-asn/1609dot2-asn/1609dot2-base-types.asn  ../scms-asn/1609dot2-asn/1609dot2-schema.asn  J2735_201603DA.ASN SEMI_v2.3.0_070616.asn`
	1. Edit the `Makefile.am.example` file and add `-DPDU=MessageFrame` prior to the `-DASN_PDU_COLLECTION` flag in the definition of the `CFLAGS` variable.
1. Check: you should have the `libasncodec.a` static library and the `converter-example` executable in the asn1c_combined subdirectory 
1. Change directory to the asn1_codec root directory.
1. The module uses the [`pugixml` library](https://pugixml.org).  This must be cloned and installed on your system.
    1. The ACM will look for the library in `/usr/local/lib` If you put it somewhere else, you must modify the
       `CMakeLists.txt` files in both the root and `src` subdirectories.
    1. The ACM will look for the headers in `/usr/local/include` Same directions about modifications apply if you put
       these somewhere else.
1. This should create (in the build directory you are in) the following executables: `acm`
1. Edit the configuration file `build/config/example.properties`
    1. Change the topics in this file for the consumer and producer (for example): `asn1.j2735.topic.consumer=j2735asn1per` and `asn1.j2735.topic.producer=j2735asn1xer`
    1. Change the broker IP address (for example): `metadata.broker.list=172.17.0.1:9092`

## Running the acm standalone and just processing an input XML file (with hex byte strings).

1. You can run the acm WITHOUT kafka to test it.
1. The test file I have been using is: `asn1_codec/data/InputData.Ieee1609Dot2Data.packed.xml`
    - If the file has newlines/pretty printed parsing fails.
	- I think this reflects most of the recent metadata file format.
1. Test from the `build` directory using the following command:
    - `$ ./acm -F -c config/example.properties ../data/InputData.Ieee1609Dot2Data.packed.xml`
        - `-F` is for file input.  The file is the operand.
        - `-c` is the configuration file (probably useless in this case, but consumed none the less).

## Running the acm with kafka

1. You can run the acm WITH kafka to test it.
1. The test file I have been using is: `asn1_codec/data/InputData.Ieee1609Dot2Data.Bsm.packed.xml`
    - If the file has newlines/pretty printed parsing fails.
	- I think this reflects most of the recent metadata file format.
1. Test from the `build` directory using the following command:
    - `$ ./acm -c config/example.properties`

# Notes, etc.

- The output decoded XML that is returned is placed under the `OdeAsn1Data/payload/data` XPath.  The `bytes` tag is removed because these are no longer bytes.
- The output decoded XML does not have the 1609.2 data only the BSM message frame.

## Documentation

This documentation is in the `README.md` file. The remaining documentation files are in the `docs` subdirectory.  The
following document will help practitioners build, test, deploy, and understand the ACM's functions:

- [ASN.1 Codec Module User Guide](docs/acm_user_manual.docx)

All stakeholders are invited to provide input to these documents. Stakeholders should direct all input on this document
to the JPO Product Owner at DOT, FHWA, or JPO. To provide feedback, we recommend that you create an "issue" in this
repository (https://github.com/usdot-jpo-ode/asn1_codec/issues). You will need a GitHub account to create an issue. If you
don’t have an account, a dialog will be presented to you to create one at no cost.

## Code Documentation

Code documentation can be generated using [Doxygen](https://www.doxygen.org) by following the commands below:

```bash
$ sudo apt install doxygen
$ cd <install root>/asn1_codec
$ doxygen
```

The documentation is in HTML and is written to the `<install root>/asn1_codec/docs/html` directory. Open `index.html` in a
browser.

