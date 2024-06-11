# Installation and Setup

## Table of Contents
1. [Docker Installation](#docker-installation)
1. [Manual Installation](#manual-installation)

## Docker Installation

### 1. Install [Docker](https://www.docker.com)

- When following the website instructions, setup the Docker repos and follow the Linux post-install instructions.
- The CE version seems to work fine.
- [Docker installation instructions](https://docs.docker.com/engine/installation/linux/ubuntu/#install-using-the-repository)

#### ORNL Specific Docker Configuration
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

Be sure to restart the docker daemon to consume the new configuration file.

```bash
$ service docker stop
$ service docker start
```

Check the configuration using the command below to confirm the updates above are taken if needed:

```bash
$ docker info
```

### 2. Configure environment variables
Configure the environment variables for the ACM to communicate with the Kafka instance. Copy or rename the `sample.env` file to `.env`.

```bash
$ cp sample.env .env
```

Edit the `.env` file to include the necessary information.

```bash
$ vi .env
```

For more information on the environment variables, see the 'Environment Variables' section in the [configuration.md](configuration.md) file.

### 3. Spin up Kafka & the ACM in Docker
To spin up the ACM and Kafka in Docker, use the following commands:

```bash
docker compose up --build
```

## Manual Installation
The ACM can be executed after following these high-level steps:

1. Obtain the necessary build tools.
1. Obtain the ACM code, submodules, and documentation. 
1. Build and install the submodules. This includes the ASN.1 compiler.
1. Extract previously generated header/implementation files.
1. Build the ACM.
1. Run the ACM.

Docker may also be used to run the ACM. The [Docker Container](#docker-installation) instructions have more information. 

### 1. Install [Git](https://git-scm.com/)

```bash
$ sudo apt install -y git
```

### 2. Install [CMake](https://cmake.org)

The ACM uses CMake to build.

```bash
$ sudo apt install -y cmake
```

### 3. Install Requirements for Autotools

The ASN.1 Compiler (asn1c) uses autotools to build.

```bash
$ sudo apt install -y autoconf
$ sudo apt install -y libtool
```

### 4. Install librdkafka

Talking to a Kafka instance, subscribing and producting to topics requires the use of a third party library. We use the librdkafka library as a c/c++ implementation. This can be installed as a package.

```bash
$ sudo apt install -y libsasl2-dev 
$ sudo apt install -y libsasl2-modules
$ sudo apt install -y libssl-dev 
$ sudo apt install -y librdkafka-dev
```

### 5. Create a base directory from which to install all the necessary components to test the ACM.

```bash
$ export GIT_REPOS=~/some/dir/you/want/to/put/this/stuff
```

### 6. Install the ACM Code, Submodules, and Documentation

```bash
$ cd $GIT_REPOS
$ git clone --recurse-submodules https://github.com/usdot-jpo-ode/asn1_codec.git
```

The ACM uses code from several other open source projects, namely `librdkafka`, `asn1c`, `pugixml`, `scms-asn`, `spdlog`, and
`catch`.  `spdlog` and `catch` are header only. The versions of these headers that were tested with the project are
included in the ACM project. The other repositories are submodules that must be built (excluding librdkafka); the `--recurse-submodules` option
retrieves these submodules. The following instructions will help you build these libraries (`asn1c` and `pugixml`).

After the ACM has been cloned the following commands can be used to update the submodules.

```bash
$ cd $GIT_REPOS/asn1_codec
$ git submodule update --init --recursive
```

### 7. Build the Submodules

#### a. Build the ASN.1 Compiler (asn1c).  

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

Note: If you are having additional problems check the [requirements page](https://github.com/vlm/asn1c/blob/master/REQUIREMENTS.md), although `bison` or `flex` may not be needed to get asn1c to build.

#### b. Build the pugixml library. 

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

### 8. Extract Previously Generated Header/Implementation Files
The `asn1c_combined` directory contains the header and implementation files that were generated from the ASN.1 compiler. These files are included in the repository, but if you need to regenerate them, follow the instructions in the [ASN.1 C File Generation](asn1c_combined/README.md) file.

The header and implementation files can be extracted by running the `doIt.sh` script in the `asn1c_combined` directory.

```bash
$ cd $GIT_REPOS/asn1_codec/asn1c_combined
$ ./doIt.sh
```

During compilation numerous source code files will be generated in or moved into this directory. It will also compile the command line tool, `converter-example`, which is not
needed, but useful.

Do not remove the header files that are generated in this directory

### 9. Build the ACM

Finally, build the ACM.  The ACM is build using CMake.

```bash
$ cd $GIT_REPOS/asn1_codec
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Additional information

The following projects are used by the ACM; however, they are header-only and those headers are included in the
ACM repository. You can pull fresh headers from the following locations:

- The ACM uses [spdlog](https://github.com/gabime/spdlog) for logging; it is a header-only library and the headers are included in the repository.
- The ACM uses [Catch](https://github.com/philsquared/Catch) for unit testing, but it is a header-only library included in the repository.
- Currently, the ACM does not use [RapidJSON](https://github.com/miloyip/rapidjson), but this header-only library may be used in the future.
