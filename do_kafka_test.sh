#!/bin/bash
./start_kafka.sh

# Wait until Kafka creates our topics.
while true; do
    ntopics=$(docker exec asn1codec_kafka_1 /opt/kafka/bin/kafka-topics.sh --list --zookeeper 172.17.0.1 | wc -l)

    if [[ $ntopics == "2" ]]; then 
        echo 'Found 2 topics:'
        docker exec asn1codec_kafka_1 /opt/kafka/bin/kafka-topics.sh --list --zookeeper 172.17.0.1 2> /dev/null
        
        break   
    fi

    sleep 1
done

./test-scripts/standalone.sh config/test/c1.properties data/producer_test_xml.txt encode 0
