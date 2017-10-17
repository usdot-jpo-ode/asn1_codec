# Installation and Setup

The ACM can be executed after following these high-level steps:

1. Obtain the necessary build tools.
1. Obtain the necessary libraries and headers needed by the ACM, and build the needed libraries. These are all open source.
1. Obtain the latest ASN.1 Compiler, and build it. It is also open source.
1. Location or purchase the necessary ASN.1 specifications.
1. Use the ASN.1 Compiler to compile the needed ASN.1 specifications into C source code.
1. Use the ASN.1 Compiler generated `Makefile.am.sample` to generate an ASN.1-specific library for the ACM.
1. Obtain the latest ACM code.
1. Build the ACM.
1. Run the ACM.

Docker may also be used to run the ACM. The [Docker Container](#using-the-docker-container) instructure have more information. 

## 1. Install [Git](https://git-scm.com/)

When asked if you wish to continue after running the command below, type 'Y'.

```bash
$ sudo apt install git
```

## 2. Install [CMake](https://cmake.org)

The ACM uses CMake to build.  When asked if you wish to continue after running the command below, type 'Y'.

```bash
$ sudo apt install cmake
```

## 2. Install Autotools Requirements

The ASN.1 Compiler (asn1c) uses autotools to build.  When asked if you wish to continue after running the command below, type 'Y'.

```bash
$ sudo apt install autoconf
$ sudo apt install libtool
```

## 8. Create a base directory from which to install all the necessary components to test the ACM.

```bash
$ export GIT_REPOS=~/some/dir/you/want/to/put/this/stuff
```

## 8. Install the ACM Code, Submodules, and Documentation

```bash
$ cd $GIT_REPOS
$ git clone https://github.com/usdot-jpo-ode/asn1_codec.git
```

The ACM uses code from several other open source projects, namely, asn1c, pugixml, spdlog, and catch.  spdlog and catch
are header only. The versions of these headers that were tested with the project are included in the asn1_codec you just
cloned. The other repositories are submodules that must be built. The following instructions will help you build these libraries (asn1c and pugixml).

```bash
$ cd $GIT_REPOS/asn1_codec
$ git submodule update --init --recursive
```

These command should pull and update the code from the submodules (asn1c and pugixml).

## 8. Build the Submodules

### Build the ASN.1 Compiler (asn1c).  

The documentation for building and installing the [ASN.1
Compiler](https://github.com/vlm/asn1c) (asn1c) is on github.  The top level documentation is in the `README.md` file in
the root directory of the repository you just pulled and refreshed.

The [ASN.1 Compiler installation instructions](https://github.com/vlm/asn1c/blob/master/INSTALL.md) can be found in the
`INSTALL.md` file.  **IMPORTANT**: On a fresh install of Ubuntu 16.04 there is one step missing from the installation
instructions in the asn1c repository. *If you skip executing `aclocal` step you may also receive this error.*

```bash
config.status: error: cannot find input file: `tests/tests-c-compiler/check-src/Makefile.in'
```

The following steps worked on a fresh intall of Ubuntu 16.04:

```bash
$ cd $GIT_REPOS/asn1_codec/asn1c
$ git pull origin master
$ aclocal
$ test ./configure || autoreconf -iv
$ ./configure
$ make
$ sudo make install
```

Note: If you are having additional problems check the [requirements
page](https://github.com/vlm/asn1c/blob/master/REQUIREMENTS.md), although I did not need `bison` or `flex` to get asn1c
to build.

### Build the pugixml library. 

The asn1_codec uses [pugixml](https://pugixml.org) to process XML (ASN.1 XER); XML is the ACM's human-readable format.
The pugixml library is built using CMake. Although the [pugixml quickstart](https://pugixml.org/docs/quickstart.html)
guide discusses building, it does not include the following simple directions:

```bash
$ cd $GIT_REPOS/asn1_codec/pugixml
$ git pull origin master
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

Note: an out of source build is also an option and works.

This should install these libraries in the usual locations, i.e., /usr/local/lib /usr/local/include

### Build the librdkafka library.

The ACM uses the [Kafka](https://kafka.apache.org) distributed streaming platform to consume and produce data. Since the
ACM and ASN.1 compiler are written in C/C++, a C++ implementation of Kafka is used. The open source library used is
called [`librdkafka`](https://github.com/edenhill/librdkafka). `librdkafka` has been included as a submodule in the
asn1_codec project. To install this module (if not already installed) use the following instructions:

```bash
$ cd $GIT_REPOS/asn1_codec/librdkafka
$ git pull origin master
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

- **NOTE**: The header files for `librdkafka` should be located in `/usr/local/include/librdkafka` and the libraries
  (static and dynamic) should be located in `/usr/local/lib`. If you put them in another location the asn1_codec may not build.

## 12. Download, Build, and Install the ASN.1 CODEC (asn1_codec)

```bash
$ cd $GIT_REPOS
$ git clone https://github.com/usdot-jpo-ode/asn1_codec.git
$ cd asn1_codec
$ ./docker_build.sh
$ ./run-with-mount.sh
```

## Additional information

The following projects are used by the ACM; however, they are header-only and those headers are included in the asn1_codec repository. You can pull fresh headers from the following locations:

- The asn1_codec uses [spdlog](https://github.com/gabime/spdlog) for logging; it is a header-only library and the headers are included in the repository.
- The asn1_codec uses [Catch](https://github.com/philsquared/Catch) for unit testing, but it is a header-only library included in the repository.
- The asn1_codec uses [RapidJSON](https://github.com/miloyip/rapidjson), but it is a header-only library included in the repository.

# Integrating with the ODE

TBD

## Docker Installation

## 4. Install [Docker](https://www.docker.com)

- When following the website instructions, setup the Docker repos and follow the Linux post-install instructions.
- The CE version seems to work fine.
- [Docker installation instructions](https://docs.docker.com/engine/installation/linux/ubuntu/#install-using-the-repository)
- *ORNL specific, but may apply to others with organizational security*
    - Correct for internal Google DNS blocking
    - As root (`$ sudo su`), create a `daemon.json` file in the `/etc/docker` directory that contains the following information:
```bash
          {
              "debug": true,
              "default-runtime": "runc",
              "dns": ["160.91.126.23","160.91.126.28”],
              "icc": true,
              "insecure-registries": [],
              "ip": "0.0.0.0",
              "log-driver": "json-file",
              "log-level": "info",
              "max-concurrent-downloads": 3,
              "max-concurrent-uploads": 5,
              "oom-score-adjust": -500
          }
```
- NOTE: The DNS IP addresses are ORNL specific.

## 5. Restart the docker daemon to consume the new configuration file.

```bash
$ service docker stop
$ service docker start
```

## 6. Check the configuration using the command below to confirm the updates above are taken if needed:

```bash
$ docker info
```

## 6. Install Docker
  Furthermore, we
recommend using Docker to build the necessary containers to build, test, and experiment with the ACM.
[Docker](https://www.docker.com) installation instructions can be found on the Docker website. These tools can be found
here:
- [Docker](https://www.docker.com)

## 7. Install Docker Compose
- Comprehensive instructions can be found on this [website](https://www.digitalocean.com/community/tutorials/how-to-install-docker-compose-on-ubuntu-16-04)
- Follow steps 1 and 2.

## 9. Install [`kafka-docker`](https://github.com/wurstmeister/kafka-docker) so kafka and zookeeper can run in a separate container.

- Get your host IP address. The address is usually listed under an ethernet adapter, e.g., `en<number>`.

```bash
$ ifconfig
$ export DOCKER_HOST_IP=<HOST IP>
$ export ASN1_CODEC_HOME=${GIT_REPOS}/asn1_codec
```
- Get the kafka and zookeeper images.

```bash
$ cd $GIT_REPOS
$ git clone https://github.com/wurstmeister/kafka-docker.git
$ cd kafka-docker
$ vim docker-compose.yml	                        // Set karka: ports: to 9092:9092
```
- The `docker-compose.yml` file may need to be changed; the ports for kafka should be 9092:9092.
- Startup the kafka and zookeeper containers and make sure they are running.

```bash
$ docker-compose up --no-recreate -d
$ docker-compose ps
```
- **When you want to stop kafka and zookeeper, execute the following commands.**

```bash
$ cd $GIT_REPOS/kafka-docker
$ docker-compose down
```

## 10. Download and install the Kafka **binary**.

-  The Kafka binary provides a producer and consumer tool that can act as surrogates for the ODE (among other items).
-  [Kafka Binary](https://kafka.apache.org/downloads)
-  [Kafka Quickstart](https://kafka.apache.org/quickstart) is a very useful reference.
-  Move and unpack the Kafka code as follows:

```bash
$ cd $GIT_REPOS
$ wget http://apache.claz.org/kafka/0.10.2.1/kafka_2.12-0.10.2.1.tgz   // mirror and kafka version may change; check website.
$ tar -xzf kafka_2.12-0.10.2.1.tgz			               // the kafka version may be different.
$ mv kafka_2.12-0.10.2.1 kafka
```


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
