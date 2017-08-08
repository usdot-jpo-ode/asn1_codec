#!/bin/sh
docker run -it -v `pwd`:/asn1_codec -e DOCKER_HOST_IP=${DOCKER_HOST_IP} asn1_codec:latest $*
