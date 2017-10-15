# Changes, etc.

- The module performs both *encoding* and *decoding* now.
- IMPORTANT: There is one executable (`acm`) that performs both encoding and decoding depending on options used when starting.
- The `-T` options is used with either `-T encode` or `-T decode` to specify the behavior of the module during
  execution.

     - One one conditional was added to make the above work, so it isn't a significant performance penalty; also the
       code was refactored to reuse several important parts.

- The decoder will using the following XPath string to search for the unsecuredData hex bytes to decode into XML:

     - `Ieee1609Dot2Data/content//unsecuredData`
     - The double slash allow this search to find the right payload in `signedData` messages and `unsecuredData`
       messages (the latter is essentially unsigned).

- The encoder will use UPER to ASN.1 encode the XML in the payload section of the metadata.

     - Right now we are just dealing with TIM MessageFrames so this should be sufficient.
     - Should be fairly easy to modify if we need to add other encodings.

- The XML path: `OdeAsn1Data/payload/dataType` text information is adjusted as it moves through the ACM.

     - After ENCODING this text is changed to: `us.dot.its.jpo.ode.model.OdeHexByteArray`
     - After DECODING this text is changed to: `us.dot.its.jpo.ode.model.OdeXml` (Note: not sure what is the write type string to use here).

- The DECODER does use the `elementType` and `encoderRule` tags to determine which types of decoding to perform.

- Both the ENCODER and DECODER will check the ASN.1 constraints for the C structures that are built as data passes
  through the module.

- I really recommend using the `-R` flag when testing.  This will start with fresh log files every time and makes it
  easier to figure out what is happening without trying to find your place in a huge log file.

     - I've attempted to annotate the logs with the function names that are performing the actions for traceability.

# Required Encoding / Decoding Rules

- IEEE 1609.2 ASN.1 is encoded as COER (canonical octet encoding rules): compiler api commands - asn_decode is used with
  the appropriate `ats_transfer_syntax` specified.
- SAE j2735 ASN.1 is encoded into UPER (unaligned packed encoding rules): compiler api commands - uper_decoder and uper_encoder.

# Branch ODE-581 Instructions

1. Clone the asn1_codec repository, and switch to the ODE-581 branch.
1. Clone the asn1c repository (shouldn't really matter where you put it, but inside the asn1_codec repos is a good
   place.
1. Follow the instructions in the asn1c repository file INSTALL.md
1. Clone the 1609dot2-asn repository on github (can be done inside of the asn1_codec library.
1. Copy \*.asn from the 1609dot2-asn directory into the asn1c_combined subdirectory in the asn1_codec repository (you
   will not need all of these, but a couple will be needed).
    1. The files that are open source have been added to the `asn1_codec\asn1c_combined` subdirectory.
1. [j2735 Specification must be purchased from SAE] Find your J2735 asn file and copy that into the asn1_codec repository.
1. The doIt.sh can perform the needed operations to compile the ASN specifications and fixup the Makefile so the CLI tool compiles. The individual commands are:
    1. Compile the ASN specifictions: `asn1c -fcompound-names -gen-PER -pdu=all 1609dot2-base-types.asn 1609dot2-schema.asn J2735_201603DA.ASN SEMI_v2.3.0_070616.asn`
	1. You will need to edit the `Makefile.am.example` file and add `-DPDU=MessageFrame` prior to the `-DASN_PDU_COLLECTION` flag in the definition of the `CFLAGS` variable.
1. Check: you should have the `libasncodec.a` static library and the `converter-example` executable in the asn1c_combined subdirectory 
1. Change directory to the asn1_codec root directory.
1. The module uses the [`pugixml` library](https://pugixml.org).  This must be cloned and installed on your system.
    1. The ACM will look for the library in `/usr/local/lib` If you put it somewhere else, you must modify the
       `CMakeLists.txt` files in both the root and `src` subdirectories.
    1. The ACM will look for the headers in `/usr/local/include` Same directions about modifications apply if you put
       these somewhere else.
1. The module also uses Catch and librdkafka.  Those instructions are the same as in the PPM.
1. Execute: `$ mkdir build`
1. Execute: `$ cd build`
1. Execute: `$ cmake ..`
1. Execute: `$ make`
1. This should create (in the build directory you are in) the following executables: `acm` and `aem`.  ONLY `acm` is working now.
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


 


