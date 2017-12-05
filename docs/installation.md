# Installation and Setup

The ACM can be executed after following these high-level steps:

1. Obtain the necessary build tools.
1. Obtain the ACM code, submodules, and documentation. 
1. Build and install the submodules. This includes the ASN.1 compiler.
1. Location or purchase the necessary ASN.1 specifications.
1. Use the ASN.1 Compiler to compile the needed ASN.1 specifications into C source code.
1. Use the ASN.1 Compiler generated `Makefile.am.sample` to generate an ASN.1-specific library for the ACM.
1. Build the ACM.
1. Run the ACM.

Docker may also be used to run the ACM. The [Docker Container](#docker-installation) instructions have more information. 

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

## 3. Install Requirements for Autotools

The ASN.1 Compiler (asn1c) uses autotools to build.  When asked if you wish to continue after running the command below, type 'Y'.

```bash
$ sudo apt install autoconf
$ sudo apt install libtool
```

## 4. Create a base directory from which to install all the necessary components to test the ACM.

```bash
$ export GIT_REPOS=~/some/dir/you/want/to/put/this/stuff
```

## 5. Install the ACM Code, Submodules, and Documentation

```bash
$ cd $GIT_REPOS
$ git clone --recurse-submodules https://github.com/usdot-jpo-ode/asn1_codec.git
```

The ACM uses code from several other open source projects, namely `librdkafka`, `asn1c`, `pugixml`, `scms-asn`, `spdlog`, and
`catch`.  `spdlog` and `catch` are header only. The versions of these headers that were tested with the project are
included in the ACM project. The other repositories are submodules that must be built; the `--recurse-submodules` option
retrieves these submodules. The following instructions will help you build these libraries (`asn1c` and `pugixml`).

After the ACM has been cloned the following commands can be used to update the submodules.

```bash
$ cd $GIT_REPOS/asn1_codec
$ git submodule update --init --recursive
```

## 6. Build the Submodules

### a. Build the ASN.1 Compiler (asn1c).  

The documentation for building and installing the [ASN.1 Compiler](https://github.com/vlm/asn1c) (asn1c) is on github.
The top level documentation is in the `README.md` file in the root directory of the repository you just pulled and
refreshed.

The [ASN.1 Compiler installation instructions](https://github.com/vlm/asn1c/blob/master/INSTALL.md) can be found in the
`INSTALL.md` file.  **IMPORTANT**: On a fresh install of Ubuntu 16.04 there is one step missing from the installation
instructions in the asn1c repository. *If you skip executing the `aclocal` step you may also receive this error.*

```bash
config.status: error: cannot find input file: `tests/tests-c-compiler/check-src/Makefile.in'
```

The following steps worked on a fresh intall of Ubuntu 16.04:

```bash
$ cd $GIT_REPOS/asn1_codec/asn1c
$ git pull origin master
$ aclocal
$ test -f ./configure || autoreconf -iv
$ ./configure
$ make
$ sudo make install
```

Note: If you are having additional problems check the [requirements page](https://github.com/vlm/asn1c/blob/master/REQUIREMENTS.md), 
although I did not need `bison` or `flex` to get asn1c to build.

### b. Build the pugixml library. 

The ACM uses [pugixml](https://pugixml.org) to process XML (ASN.1 XER); XML is the ACM's human-readable format.
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

This should install these libraries in the usual locations, i.e., `/usr/local/lib` `/usr/local/include`

### c. Build the librdkafka library.

The ACM uses the [Kafka](https://kafka.apache.org) distributed streaming platform to consume and produce data. Since the
ACM and ASN.1 compiler are written in C/C++, a C++ implementation of Kafka is used. The open source library used is
called [`librdkafka`](https://github.com/edenhill/librdkafka). `librdkafka` has been included as a submodule in the
ACM project. To install this module (if not already installed) use the following instructions:

```bash
$ cd $GIT_REPOS/asn1_codec/librdkafka
$ git pull origin master
$ ./configure
$ make
$ sudo make install
```

This should install the libraries in `/usr/local/lib/`. Two libraries: `librdkafka++.a` and `librdkafka.a`
This should install the headers in `/usr/local/include/librdkafka/`. Two headers: `rdkafka.h` and `rdkafkacpp.h`

Note: if you put these in a different location, the ACM may not build.
Note: There is a CMakeLists.txt file, but using CMake did not work.

## 7. Compile the J2735 and IEEE 1609.2 ASN.1 Specification Files and Build the CV ASN.1 Codec Library

The ACM project's `asn1c_combined` subdirectory is where a library that performs connected vehicle specific ASN.1
encoding and decoding will be built. More specifically, this library decodes various ASN.1 binary encodings (e.g., UPER,
OER) into C structures that conform the ASN.1 specification files that were compiled. It also encodes the C structures
back into other encodings (e.g., XER, or XML). This subdirectory must contain the ASN.1 specification files needed to
encode and decode connected vehicle communications. It contains a script to compile the specifications and build the
library (and a command line tool that can be used independently to encode and decode ASN.1 files).

### a. Obtain the Necessary ASN.1 Specification Files

The above subdirectory must contain the below specification files; all of these are included except the j2735
specification that needs to be [purchased from SAE](https://saemobilus.sae.org/content/j2735_201603). These
specifications are subject to change and should be updated periodically.

- [IEEE 1609.2](https://github.com/wwhyte-si/1609dot2-asn.git)
     - `1609dot2-base-types.asn`
     - `1609dot2-schema.asn`
- Southeast Michigan Testbed Specification
     - `SEMI_v2.3.0_070616.asn`
- SAE J2735
	 - `J2735_201603DA.ASN`

### b. Compile the Specifications and Build the Library

```bash
$ cd $GIT_REPOS/asn1_codec/asn1c_combined
$ ./doIt.sh
```

During compilation numerous source code files will be generated in or moved into this directory. It will also create the
static library, `libasncodec.a`, and the command line tool, `converter-example`. The latter is not
needed, but useful. Do not remove the header files that are generated in this directory

## 8. Build the ACM

Finally, build the ACM.  The ACM is build using CMake.

```bash
$ cd $GIT_REPOS/asn1_codec
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Additional information

The following projects are used by the ACM; however, they are header-only and those headers are included in the
ACM repository. You can pull fresh headers from the following locations:

- The ACM uses [spdlog](https://github.com/gabime/spdlog) for logging; it is a header-only library and the headers are included in the repository.
- The ACM uses [Catch](https://github.com/philsquared/Catch) for unit testing, but it is a header-only library included in the repository.
- Currently, the ACM does not use [RapidJSON](https://github.com/miloyip/rapidjson), but this header-only library may be used in the future.

# Docker Installation

## 1. Install [Docker](https://www.docker.com)

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
              "dns": ["160.91.126.23","160.91.126.28"],
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

## 2. Restart the docker daemon to consume the new configuration file.

```bash
$ service docker stop
$ service docker start
```

## 3. Check the configuration using the command below to confirm the updates above are taken if needed:

```bash
$ docker info
```

## 4. Install Docker Compose
- Comprehensive instructions can be found on this [website](https://www.digitalocean.com/community/tutorials/how-to-install-docker-compose-on-ubuntu-16-04)
- Follow steps 1 and 2.

## 5. Install [`kafka-docker`](https://github.com/wurstmeister/kafka-docker) so kafka and zookeeper can run in a separate container.

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

## 6. Download and install the Kafka **binary**.

-  The Kafka binary provides a producer and consumer tool that can act as surrogates for the ODE (among other items).
-  [Kafka Binary](https://kafka.apache.org/downloads)
-  [Kafka Quickstart](https://kafka.apache.org/quickstart) is a very useful reference.
-  Move and unpack the Kafka code as follows:

```bash
$ cd $GIT_REPOS
$ wget http://apache.claz.org/kafka/0.10.2.1/kafka_2.12-0.10.2.1.tgz   // mirror and kafka version may change; check website.
$ tar -xzf kafka_2.12-0.10.2.1.tgz			                           // the kafka version may be different.
$ mv kafka_2.12-0.10.2.1 kafka
```




