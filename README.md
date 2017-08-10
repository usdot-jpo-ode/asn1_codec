Master: [![Build Status](https://travis-ci.org/usdot-jpo-ode/asn1_codec.svg?branch=master)](https://travis-ci.org/usdot-jpo-ode/asn1_codec) [![Quality Gate](https://sonarqube.com/api/badges/gate?key=asn1_codec)](https://sonarqube.com/dashboard?id=asn1_codec-key)

# asn1_codec

## Table of Contents

1. [Release Notes](#release-notes)
2. [Documentation](#documentation)
3. [Development and Collaboration Tools](#development-and-collaboration-tools)
3. [Getting Started](#getting-started)
4. [Installation](docs/installation.md)
5. [Configuration and Operation](docs/configuration.md)
6. [Testing](docs/testing.md)
7. [Development](docs/coding-standards.md)

## Release Notes


# Documentation

## Code Documentation

Code documentation can be generated using [Doxygen](https://www.doxygen.org) by following the commands below:

```bash
$ sudo apt install doxygen
$ cd <install root>/asn1_codec
$ doxygen
```

The documentation is in HTML and is written to the `<install root>/asn1_codec/docs/html` directory. Open `index.html` in a browser.

# Development and Collaboration Tools

## Source Repositories - GitHub

- https://github.com/usdot-jpo-ode/asn1_codec
- `git@github.com:usdot-jpo-ode/asn1_codec.git`

## Agile Project Management - Jira
https://usdotjpoode.atlassian.net/secure/Dashboard.jspa

## Continuous Integration and Delivery

The asn1_codec is tested using [Travis Continuous Integration](https://travis-ci.org).

# Getting Started

## Prerequisites

You will need Git to obtain the code and documents in this repository.
Furthermore, we recommend using Docker to build the necessary containers to
build, test, and experiment with the asn1_codec. The [Docker](#docker) instructions can be found in that section.

- [Git](https://git-scm.com/)
- [Docker](https://www.docker.com)

You can find more information in our [installation and setup](docs/installation.md) directions.

## Getting the Source Code

See the installation and setup instructions unless you just want to examine the code.

**Step 1.** Disable Git `core.autocrlf` (Only the First Time)

   **NOTE**: If running on Windows, please make sure that your global git config is
   set up to not convert End-of-Line characters during checkout. This is important
   for building docker images correctly.

```bash
git config --global core.autocrlf false
```

**Step 2.** Clone the source code from GitHub repositories using Git commands:

```bash
git clone https://github.com/usdot-jpo-ode/asn1_codec.git
```
