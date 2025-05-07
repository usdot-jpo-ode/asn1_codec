# Abstract Syntax Notation One (ASN.1) Codec Module for the Operational Data Environment (ODE)

The ASN.1 Codec Module (ACM) processes Kafka data streams that preset [ODE
Metadata](https://github.com/usdot-jpo-ode/jpo-ode/blob/develop/docs/metadata_standards.md) wrapped ASN.1 data.  It can perform
one of three functions depending on how it is started:

1. **Decode**: This function is used to process messages *from* the connected
vehicle environment *to* ODE subscribers. Specifically, the ACM extacts binary
data from consumed messages (ODE Metadata Messages) and decodes the binary
ASN.1 data into a structure that is subsequently encoded into an alternative
format more suitable for ODE subscribers (currently XML using XER).

2. **Encode**: This function is used to process messages *from* the ODE *to*
the connected vehicle environment. Specifically, the ACM extracts
human-readable data from ODE Metadata and decodes it into a structure that
is subsequently *encoded into ASN.1 binary data*.

3. **HTTP Server**: This function is used to run an HTTP Server for applications
that need to decode ASN.1 messages using a request/response paradigm.  Currently, the Kafka
consumers and producers do not run in this mode.

![ASN.1 Codec Operations](docs/graphics/asn1codec-operations.png)

## Table of Contents

**README.md**
1. [Release Notes](#release-notes)
2. [Getting Involved](#getting-involved)
3. [Documentation](#documentation)
4. [Generating C Files from ASN.1 Definitions](#generating-c-files-from-asn1-definitions)
5. [Confluent Cloud Integration](#confluent-cloud-integration)
6. [HTTP Server](#http-server)

**Other Documents**
1. [Installation](docs/installation.md)
2. [Configuration and Operation](docs/configuration.md)
3. [Interface](docs/interface.md)
4. [Testing](docs/testing.md)

## Release Notes
The current version and release history of the asn1_codec: [asn1_codec Release Notes](<docs/Release_notes.md>)

## Getting Involved

This project is sponsored by the U.S. Department of Transportation and supports Operational Data Environment data type
conversions. Here are some ways you can start getting involved in this project:

- **Pull the code and check it out!**  The ASN.1 Codec project uses the [Pull Request Model](https://help.github.com/articles/using-pull-requests).
    - Github has [instructions](https://help.github.com/articles/signing-up-for-a-new-github-account) for setting up an account and getting started with repositories.
- If you would like to improve this code base or the documentation, [fork the project](https://github.com/usdot-jpo-ode/asn1_codec#fork-destination-box) and submit a pull request.
- If you find a problem with the code or the documentation, please submit an [issue](https://github.com/usdot-jpo-ode/asn1_codec/issues/new).

### Introduction

This project uses the [Pull Request Model](https://help.github.com/articles/using-pull-requests). This involves the following project components:

- The usdot-jpo-ode organization project's [master branch](https://github.com/usdot-jpo-ode/asn1_codec).
- A personal GitHub account.
- A fork of a project release tag or master branch in your personal GitHub account.

A high level overview of our model and these components is as follows. All work will be submitted via pull requests.
Developers will work on branches on their personal machines (local clients), push these branches to their **personal GitHub repos** and issue a pull
request to the organization asn1_codec project. One the project's main developers must review the Pull Request and merge it
or, if there are issues, discuss them with the submitter. This will ensure that the developers have a better
understanding of the code base *and* we catch problems before they enter `master`. The following process should be followed:

### Initial Setup

1. If you do not have one yet, create a personal (or organization) account on GitHub (assume your account name is `<your-github-account-name>`).
1. Log into your personal (or organization) account.
1. Fork [asn1_codec](https://github.com/usdot-jpo-ode/asn1_codec/fork) into your personal GitHub account.
1. On your computer (local client), clone the master branch from you GitHub account:
```bash
$ git clone https://github.com/<your-github-account-name>/asn1_codec.git
```

### Additional Resources for Initial Setup
  
- [About Git Version Control](http://git-scm.com/book/en/v2/Getting-Started-About-Version-Control)
- [First-Time Git Setup](http://git-scm.com/book/en/Getting-Started-First-Time-Git-Setup)
- [Article on Forking](https://help.github.com/articles/fork-a-repo)

## Documentation

This documentation is in the `README.md` file. Additional information can be found using the links in the [Table of
Contents](#table-of-contents).  All stakeholders are invited to provide input to these documents. Stakeholders should
direct all input on this document to the JPO Product Owner at DOT, FHWA, or JPO. 

### Code Documentation

Code documentation can be generated using [Doxygen](https://www.doxygen.org) by following the commands below:

```bash
$ sudo apt install doxygen
$ cd <install root>/asn1_codec
$ doxygen
```

The documentation is in HTML and is written to the `<install root>/asn1_codec/docs/html` directory. Open `index.html` in a
browser.

## Generating C Files from ASN.1 Definitions
Check here for instructions on how to generate C files from ASN.1 definitions: [ASN.1 C File Generation](asn1c_combined/README.md)

This should only be necessary if the ASN.1 definitions change. The generated files are already included in the repository.

## Confluent Cloud Integration
Rather than using a local kafka instance, the ACM can utilize an instance of kafka hosted by Confluent Cloud via SASL.

### Environment variables
#### Purpose & Usage
- The DOCKER_HOST_IP environment variable is used to communicate with the bootstrap server that the instance of Kafka is running on.
- The KAFKA_TYPE environment variable specifies what type of kafka connection will be attempted and is used to check if Confluent should be utilized.
- The CONFLUENT_KEY and CONFLUENT_SECRET environment variables are used to authenticate with the bootstrap server.

#### Values
- DOCKER_HOST_IP must be set to the bootstrap server address (excluding the port)
- KAFKA_TYPE must be set to "CONFLUENT"
- CONFLUENT_KEY must be set to the API key being utilized for CC
- CONFLUENT_SECRET must be set to the API secret being utilized for CC

### CC Docker Compose File
There is a provided docker-compose file (docker-compose-confluent-cloud.yml) that passes the above environment variables into the container that gets created. Further, this file doesn't spin up a local kafka instance since it is not required.

### Note
This has only been tested with Confluent Cloud but technically all SASL authenticated Kafka brokers can be reached using this method.

## HTTP Server

The application can also be run as an HTTP server.  This mode uses the same Docker image as the Kafka consumer and producer modes.  Set the environment variable `ACM_HTTP_SERVER` to `true` to enable running in this mode.

The docker-compose-server-*.yml files can be used to start up in HTTP server mode, for example:
```bash
docker compose -f docker-compose-server.yml up --build -d
```
If testing on Windows, start up Docker Desktop, then issue the above "docker compose" command from Powershell It may not work from the WSL shell.

### Environment Variables
In HTTP server mode, the following additional environment variables are recognized:
- `ACM_HTTP_SERVER` Set to true to start up in HTTP server mode, or omit to run in Kafka mode.
- `ACM_HTTP_SERVER_PORT` The port to listen on. Default 9999.
- `ACM_HTTP_SERVER_CONCURRENCY` The number of threads for the server to use. Default 4.

### REST Endpoints
Currently, two endpoints are available to convert J2735 messages from UPER to XER:
- `POST /j2735/uper/xer` 
  - Converts one message
- `POST /batch/j2735/uper/xer`
  - Converts a batch of messages

### Integration Tests
Integration test for the REST endpoints are available in the [http-test](http-test/README.md) folder.

## Performance note on the Linux images
The main container image defined by the `Dockerfile` is based on Alpine Linux.  It was noted while developing the HTTP server feature that the performance of the application was significantly slower in the multithreaded context required by the HTTP server than when run on other Linux distributions, such as Amazon Linux (which is similar to CentOS).  In order to mitigate this issue, the Alpine Dockerfile now uses the [jemalloc](https://jemalloc.net/) memory allocator instead of the Alpine default allocator.  The main Docker image deployed to Github still is based on Alpine (with jemalloc), but an additional Dockerfile, `Dockerfile.amazonlinux` for Amazon Linux is included in this repository for those who might wish to try it as an alternative.  Some benchmark data comparing Alpine Linux with various memory allocators, and Amazon Linux, are shown here:

### Time for HTTP endpoint to decode large batches of 61735 messages each (seconds per batch)*:
|OS| Allocator                                         |Single Thread|Multithreaded (4 cores)|
|--|---------------------------------------------------|-------------|-----------------------|
|Amazon Linux 2023| [glibc system](https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html)|2.3|3.3|
|Alpine 3.18| [musl system](https://musl.libc.org/)             |4.8|12.1|
|Alpine 3.18| [jemalloc](https://jemalloc.net/)                 |3.4|4.5|
|Alpine 3.18| [mimalloc](https://github.com/microsoft/mimalloc) |3.6|4.5|

*Includes decoding time, not network transfer time.

