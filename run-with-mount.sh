#!/bin/sh
docker run -it -v ${DOCKER_SHARED_VOLUME}:/asn1_codec_share -e DOCKER_HOST_IP=${DOCKER_HOST_IP} asn1_codec:latest $*
