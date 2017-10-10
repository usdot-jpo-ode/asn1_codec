# Branch ODE-581 Instructions

1. Clone the asn1_codec repository, and switch to the ODE-581 branch.
1. Clone the asn1c repository (shouldn't really matter where you put it, but inside the asn1_codec repos is a good
   place.
1. Follow the instructions in the asn1c repository file INSTALL.md
1. Clone the 1609dot2-asn repository on github (can be done inside of the asn1_codec library.
1. Copy \*.asn from the 1609dot2-asn directory into the asn1c_combined subdirectory in the asn1_codec repository (you
   will not need all of these, but a couple will be needed).
1. Find your J2735 asn file and copy that into the asn1_codec repository.
1. The doIt.sh can perform the needed operations to compile the ASN specifications and fixup the Makefile so the CLI tool compiles. The individual commands are:
    1. Compile the ASN specifictions: `asn1c -fcompound-names -gen-PER -pdu=all 1609dot2-base-types.asn 1609dot2-schema.asn J2735_201603DA.ASN`
	1. You will need to edit the `Makefile.am.example` file and add `-DPDU=MessageFrame` prior to the `-DASN_PDU_COLLECTION` flag in the definition of the `CFLAGS` variable.
1. Check: you should have the `libasncodec.a` static library and the `converter-example` executable in the asn1c_combined subdirectory 
1. Change directory to the asn1_codec root directory.
1. Execute: `$ mkdir build`
1. Execute: `$ cd build`
1. Execute: `$ cmake ..`
1. Execute: `$ make`
1. This should create (in the build directory you are in) the following executables: `adm` and `aem`.  ONLY `adm` is working now.
1. Edit the configuration file `build/config/example.properties`
    1. Change the topics in this file for the consumer and producer (for example): `asn1.j2735.topic.consumer=j2735asn1per` and `asn1.j2735.topic.producer=j2735asn1xer`
    1. Change the broker IP address (for example): `metadata.broker.list=172.17.0.1:9092`

## Running the adm standalone and just processing an input XML file (with hex byte strings).

1. You can run the adm WITHOUT kafka to test it.
1. The test file I have been using is: `asn1_codec/data/InputData.Ieee1609Dot2Data.packed.xml`
    - If the file has newlines/pretty printed parsing fails.
	- I think this reflects most of the recent metadata file format.
1. Test from the `build` directory using the following command:
    - `$ ./adm -F -c config/example.properties ../data/InputData.Ieee1609Dot2Data.packed.xml`
        - `-F` is for file input.  The file is the operand.
        - `-c` is the configuration file (probably useless in this case, but consumed none the less).

## Running the adm with kafka

1. You can run the adm WITHOUT kafka to test it.
1. The test file I have been using is: `asn1_codec/data/InputData.Ieee1609Dot2Data.packed.xml`
    - If the file has newlines/pretty printed parsing fails.
	- I think this reflects most of the recent metadata file format.
1. Test from the `build` directory using the following command:
    - `$ ./adm -c config/example.properties`

# Notes, etc.

- The output decoded XML that is returned is placed under the `OdeAsn1Data/payload/data` XPath.  The `bytes` tag is removed because these are no longer bytes.
- The output decoded XML does not have the 1609.2 data only the BSM message frame.


 


