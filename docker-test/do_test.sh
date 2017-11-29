#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib

broker=172.17.0.1:9092

echo "**************************"
echo "Producing Data ..."
echo "**************************"
cat /asn1_codec_data/test.data | /asn1_codec/docker-test/test_in.py | /build/kafka-test/kafka_tool -P -b $broker -p 0 -t j2735asn1per 2> priv.err

# Start the DI consumer.
offset=$1

echo "**************************"
echo "Consuming Data at offset "$offset "..." 
echo "**************************"

while true; do
    /build/kafka-test/kafka_tool -C -b $broker -p 0 -t j2735asn1xer -e -o $offset 2> con.err | /asn1_codec/docker-test/test_out.py > tmp.out

    lines=$(cat tmp.out | wc -l)

    if [[ $lines != "0" ]]; then 
        cat tmp.out

        break
    fi
done
