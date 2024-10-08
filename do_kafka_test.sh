#!/bin/bash

# This script tests the ACM against a kafka cluster. It starts the Kafka cluster with `start_kafka.sh`,
# ensures that the topics are created, builds the ACM docker image, initiates tests using
# `test-scripts/standalone.sh`, and then stops the Kafka cluster with `stop_kafka.sh`.

CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

ACM_CONTAINER_NAME=test_acm_instance
ACM_IMAGE_TAG=do-kafka-test-acm-image
ACM_IMAGE_NAME=asn1_codec-acm

numTests=3
numFailures=0 # used to keep track of the number of failed tests for the summary

setup() {
    if [ -z $DOCKER_HOST_IP ]
    then
        export DOCKER_HOST_IP=$(ifconfig | grep -A 1 'inet ' | grep -v 'inet6\|127.0.0.1' | awk '{print $2}' | grep -E '^172\.1[6-9]\.|^172\.2[0-9]\.|^172\.3[0-1]\.|^192\.168\.' | head -n 1)
    fi
    if [ -z $DOCKER_HOST_IP ]
    then
        echo "DOCKER_HOST_IP is not set and could not be determined. Exiting."
        exit 1
    fi

    # print setup info
    echo "== Setup Info =="
    echo "DOCKER_HOST_IP: $DOCKER_HOST_IP"
    echo "KAFKA_CONTAINER_NAME is resolved dynamically"
    echo "ACM_CONTAINER_NAME: $ACM_CONTAINER_NAME"
    echo "ACM_IMAGE_TAG: $ACM_IMAGE_TAG"
    echo "ACM_IMAGE_NAME: $ACM_IMAGE_NAME"
    echo "========="

    ./start_kafka.sh
}

waitForKafkaToCreateTopics() {
    maxAttempts=100
    attempts=0
    KAFKA_CONTAINER_NAME=$(docker ps --format '{{.Names}}' | grep kafka)
    while true; do
        attempts=$((attempts+1))
        if [ $(docker ps | grep $KAFKA_CONTAINER_NAME | wc -l) = "0" ]; then
            echo "Kafka container '$KAFKA_CONTAINER_NAME' is not running. Exiting."
            ./stop_kafka.sh
            exit 1
        fi

        ltopics=$(docker exec -it $KAFKA_CONTAINER_NAME /opt/kafka/bin/kafka-topics.sh --list --zookeeper 172.17.0.1)
        allTopicsCreated=true
        if [ $(echo $ltopics | grep "topic.Asn1DecoderInput" | wc -l) = "0" ]; then
            allTopicsCreated=false
        elif [ $(echo $ltopics | grep "topic.Asn1DecoderOutput" | wc -l) = "0" ]; then
            allTopicsCreated=false
        elif [ $(echo $ltopics | grep "topic.Asn1EncoderInput" | wc -l) = "0" ]; then
            allTopicsCreated=false
        elif [ $(echo $ltopics | grep "topic.Asn1EncoderOutput" | wc -l) = "0" ]; then
            allTopicsCreated=false
        fi
        
        if [ $allTopicsCreated = true ]; then
            echo "Kafka has created all required topics"
            break
        fi

        sleep 1

        if [ $attempts -ge $maxAttempts ]; then
            echo "Kafka has not created all required topics after $maxAttempts attempts. Exiting."
            ./stop_kafka.sh
            exit 1
        fi
    done
}

buildACMImage() {
    echo "== Building ACM Image =="
    docker build . -t $ACM_IMAGE_NAME:$ACM_IMAGE_TAG -f Dockerfile.testing
}

run_tests() {
    echo "== Running Tests =="

    offset=0

    echo ""
    echo -e $YELLOW"Running test 1/$numTests - Encode AdvisorySituationData containing HEX-encoded BSM"$NC
    echo ""
    ./test-scripts/standalone.sh config/test/c1.properties data/producer_test1_xml.txt encode $offset
    if [ $? -eq 0 ]; then
        echo -e $GREEN"Test 1 passed"$NC
    else
        echo -e $RED"Test 1 failed"$NC
        numFailures=$((numFailures+1))
    fi

    offset=$((offset+1))

    echo ""
    echo -e $YELLOW"Running test 2/$numTests - Encode Ieee1609Dot2Data containing HEX-encoded BSM"$NC
    echo ""
    ./test-scripts/standalone.sh config/test/c1.properties data/producer_test2_xml.txt encode $offset
    if [ $? -eq 0 ]; then
        echo -e $GREEN"Test 2 passed"$NC
    else
        echo -e $RED"Test 2 failed"$NC
        numFailures=$((numFailures+1))
    fi

    offset=$((offset+1))

    echo ""
    echo -e $YELLOW"Running test 3/$numTests - Encode AdvisorySituationData containing Ieee1609Dot2Data containing HEX-encoded BSM"$NC
    echo ""
    ./test-scripts/standalone.sh config/test/c1.properties data/producer_test3_xml.txt encode $offset
    if [ $? -eq 0 ]; then
        echo -e $GREEN"Test 3 passed"$NC
    else
        echo -e $RED"Test 3 failed"$NC
        numFailures=$((numFailures+1))
    fi
}

cleanup() {
    echo "== Cleaning Up =="
    ./stop_kafka.sh

}

run() {
    numberOfSteps=5
    echo ""
    echo -e $CYAN"Step 1/$numberOfSteps: Set up test environment"$NC
    setup

    echo ""
    echo -e $CYAN"Step 2/$numberOfSteps: Wait for Kafka to create topics"$NC
    waitForKafkaToCreateTopics

    echo ""
    echo -e $CYAN"Step 3/$numberOfSteps: Build ACM image"$NC
    buildACMImage

    echo ""
    echo -e $CYAN"Step 4/$numberOfSteps: Run tests"$NC
    run_tests

    echo ""
    echo -e $CYAN"Step 5/$numberOfSteps: Clean up test environment"$NC
    cleanup

    printTestSummary
}

printTestSummary() {
    echo ""
    echo -e $CYAN"== Tests Summary =="$NC
    if [ $numFailures -eq 0 ]; then
        echo -e $GREEN"All tests passed"$NC
    else
        echo -e $RED"$numFailures/$numTests tests failed"$NC
    fi
    echo ""
}

echo ""

run

echo ""