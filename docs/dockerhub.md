# asn1_codec:

## GitHub Repository Link
https://github.com/usdot-jpo-ode/asn1_codec

## Purpose
The purpose of the asn1_codec submodule is to encode and decode streams of connected vehicle messages to and from the Asn.1 format.

## How to pull the latest image
The latest image can be pulled using the following command:
> docker pull usdotjpoode/asn1_codec:develop

## Required environment variables
The image expects the following environment variables to be set:
- DOCKER_HOST_IP

## Direct Dependencies
The ACM will fail to start up if the following containers are not already present:
- Kafka
- Zookeeper (relied on by Kafka)

## Indirect Dependencies
The ACM will not receive messages to process if the ODE is not running.

## Example docker-compose.yml with direct dependencies:
```
version: '2'
services:
  zookeeper:
    image: wurstmeister/zookeeper
    ports:
      - "2181:2181"

  kafka:
    image: wurstmeister/kafka
    ports:
      - "9092:9092"
    environment:
      KAFKA_ADVERTISED_HOST_NAME: ${DOCKER_HOST_IP}
      KAFKA_ZOOKEEPER_CONNECT: zookeeper:2181
      KAFKA_CREATE_TOPICS: "j2735asn1xer:1:1,j2735asn1per:1:1,topic.Asn1DecoderInput:1:1, topic.Asn1DecoderOutput:1:1"
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock

  asn1_codec:
    image: usdotjpoode/asn1_codec:release_q3
    ports:
      - "8080:8080"
      - "9090:9090"
    depends_on:
      - kafka
    links:
      - kafka
    environment:
      # required
      DOCKER_HOST_IP: ${DOCKER_HOST_IP}
      ACM_CONFIG_FILE: adm.properties
      # optional
      KAFKA_TYPE: "ON_PREM"
      ACM_LOG_TO_CONSOLE: true
      ACM_LOG_TO_FILE: false
      ACM_LOG_LEVEL: info
    restart: on-failure
```

## Expected startup output
The latest logs should look something like this:
```
asn1_codec-asn1_codec-1  | %4|1699550920.992|CONFWARN|rdkafka#producer-2| [thrd:app]: Configuration property group.id is a consumer property and will be ignored by this producer instance
asn1_codec-asn1_codec-1  | %4|1699550920.992|CONFWARN|rdkafka#producer-2| [thrd:app]: Configuration property fetch.message.max.bytes is a consumer property and will be ignored by this producer instance
asn1_codec-asn1_codec-1  | %4|1699550920.992|CONFWARN|rdkafka#producer-2| [thrd:app]: Configuration property auto.offset.reset is a consumer property and will be ignored by this producer instance
asn1_codec-asn1_codec-1  | %3|1699550920.992|ERROR|rdkafka#consumer-1| [thrd:app]: rdkafka#consumer-1: 192.168.0.243:9092/bootstrap: Disconnected while requesting ApiVersion: might be caused by incorrect security.protocol configuration (connecting to a SSL listener?) or broker version is < 0.10 (see api.version.request) (after 1ms in state APIVERSION_QUERY)
asn1_codec-asn1_codec-1  | %3|1699550920.992|ERROR|rdkafka#consumer-1| [thrd:app]: rdkafka#consumer-1: 192.168.0.243:9092/bootstrap: Disconnected while requesting ApiVersion: might be caused by incorrect security.protocol configuration (connecting to a SSL listener?) or broker version is < 0.10 (see api.version.request) (after 1ms in state APIVERSION_QUERY, 1 identical error(s) suppressed)
```