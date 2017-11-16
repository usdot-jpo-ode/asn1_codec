# ACM Operation

The ASN.1 Codec Module (ACM) processes Kafka data streams that preset [ODE
Metadata](http://github.com/usdot-jpo-ode/jpo-ode/blob/develop/docs/Metadata_v3.md) wrapped ASN.1 data.  It can perform
one of two functions depending on how it is started:

1. **Decode**: This function is used to process messages *from* the connected
vehicle environment *to* ODE subscribers. Specifically, the ACM extacts binary
data from consumed messages (ODE Metatdata Messages) and decodes the binary
ASN.1 data into a structure that is subsequently encoded into an alternative
format more suitable for ODE subscribers (currently XML using XER).

1. **Encode**: This function is used to process messages *from* the ODE *to*
the connected vehicle environment. Specifically, the ACM extracts
human-readable data from ODE Metadata and decodes it into a structure that
is subsequently *encoded into ASN.1 binary data*.

![ASN.1 Codec Operations](https://github.com/usdot-jpo-ode/asn1_codec/blob/master/docs/graphics/asn1codec-operations.png)

## ACM Command Line Options

The ACM can be started by specifying only the configuration file. Command line options are also available. **Command
line options override parameters specified in the configuration file.** The following command line options are available:

```bash
-T | --codec-type      : The type of codec to use: decode or encode; defaults to decode
-h | --help            : print out some help
-e | --elog            : Error log file name.
-i | --ilog            : Information log file name.
-R | --log-rm          : Remove specified/default log files if they exist.
-D | --log-dir         : Directory for the log files.
-F | --infile          : accept a file and bypass kafka.
-t | --produce-topic   : The name of the topic to produce.
-p | --partition       : Consumer topic partition from which to read.
-C | --config-check    : Check the configuration file contents and output the settings.
-o | --offset          : Byte offset to start reading in the consumed topic.
-d | --debug           : debug level.
-c | --config          : Configuration file name and path.
-g | --group           : Consumer group identifier
-b | --broker          : Broker address (localhost:9092)
-x | --exit            : Exit consumer when last message in partition has been received.
-v | --log-level       : The info log level [trace,debug,info,warning,error,critical,off]
```

# ACM Deployment

Once the ACM is [installed and configured](installation.md) it operates as a background service.  The ACM can be started
before or after other services. If started before the other services, it may produce some error messages while it waits
to connect.  The following command will start the ACM as a decoder based on a configuration file.

```
$ ./acm -c <configuration file> -T decode
```

Multiple ACM processes can be started. Each ACM will use its own configuration file.  If you want to
deploy a decoder and an encoder, start two separate ACM services with different `-T` options. In this case different
topics should be specified in the configuration files.

# ACM ASN.1 Data Sources

The ACM receives XML data from the ODE; the schema for this XML message is described in
[Metadata.md](https://github.com/usdot-jpo-ode/jpo-ode/blob/develop/docs/Metadata_v3.md) on the ODE. Currently, the ACM
extracts either a hex encoded string or an XML child document in the `\\payload\data` branch and decodes or encodes that
data into the desired output form. The converted data is re-inserted into the XML document and that document is produced
to the output topic. Metadata fields determine which type of encoding/decoding is needed and how to search for the
correct data within the decoded documents.

When decoding data from the CV environment, the ACM processes either IEEE 1609.2 wrapped J2735 MessageFrames (any type)
or MessageFrames by themselves. If 1609.2 wrapped, the ACM finds and extracts the `unsecuredData` to further decode the
J2735 MessageFrame. The decoder uses the `elementType` and `encoderRule` tags to determine which types of decoding to
perform.

When encoding data, the ACM can encode combinations Advisory Situation Data, IEEE 1609.2, and J2735 MessageFrames. 
Advisory Situation Data and IEEE 1609.2 can both contain a wrapped J2735 MessageFrame, and Advisory Situation Data
frames can contained wrapped IEEE 1609.2 data. Therefore, there are 7 possible combinations of data types the encoding module 
can handle:

- J2735 MessageFrame
- Advisory Situation Data*
- IEEE 1609.2*
- Advisory Situation Data wrapping a J2735 MessageFrame
- IEEE 1609.2 wrapping a J2735 MessageFrame
- Advisory Situation Data wrapping IEEE 1609.2* data
- Advisory Situation Data wrapping IEEE 1609.2 wrapping a J2735 MessageFrame

\* Denotes the message should already contain hex data, according the the ASN.1 specification for that message.
For instance, IEEE 1609.2 must contain hex data in its `unsecuredData` tag. If the hex data is missing or invalid, 
the ACM with likely generate an error when doing constraint checking.

- After ENCODING this text is changed to: `us.dot.its.jpo.ode.model.OdeHexByteArray`
- After DECODING this text is changed to: `us.dot.its.jpo.ode.model.OdeXml`

Both the ENCODER and DECODER will check the ASN.1 constraints for the C structures that are built as data passes through
the module.

# ACM Kafka Limitations

With regard to the Apache Kafka architecture, each ACM process does **not** provide a way to take advantage of Kafka's scalable
architecture. In other words, each ACM process will consume data from a single Kafka topic and a single partition within
that topic. One way to consume topics with multiple partitions is to launch one ACM process for each partition; the
configuration file will allow you to designate the partition. In the future, the ACM may be updated to automatically
handle multiple partitions within a single topic.

# ACM Logging

ACM operations are logged to two files: an information log and an error log.  The files are rotating log files, i.e., a set number of log files will
be used to record the ACM's information. By default, these files are located in a `logs` directory from where the ACM is launched and the files are
named `log.info` and `log.error`. The maximum size of a `log.info` files is 5MB and 5 files are rotated. The maximum size of a `log.error` file is 2MB
and 2 files are rotated. Logging configuration is controlled through the command line, not through the configuration file. The following operation are available:

- `-R` : When the ACM starts remove any log files having either the default or user specified names; otherwise, new log entries will be appended to existing files.

- `-D` : The directory where the log files should be written. This can be relative or absolute. If the directory does not exist, it will be created.

- `-e` : The error log file's name.

- `-i` : The information log file's name.

- `-v` : The minimum level of message to write to the information log. From lowest to highest, the message levels are `off`, `trace`, `debug`, `info`,
         `warning`, `error`, `critical`. As an example, if you specify `info` then all messages that are `info, warning, error, or critical` will be written to
         the log.

The information log will write the configuration it will use as `info` messages when it starts.  All log messages are
preceeded with a date and time stamp and the level of the log message.

```
[171011 18:25:55.221276] [trace] starting configure()
[171011 18:25:55.221341] [info] using configuration file: config/example.properties
[171011 18:25:55.221407] [info] kafka configuration: group.id = 0
[171011 18:25:55.221413] [info] ASN1_Codec configuration: asn1.j2735.topic.consumer = j2735asn1per 
[171011 18:25:55.221417] [info] ASN1_Codec configuration: asn1.j2735.topic.producer = j2735asn1xer
[171011 18:25:55.221420] [info] ASN1_Codec configuration: asn1.j2735.consumer.timeout.ms = 5000
[171011 18:25:55.221423] [info] ASN1_Codec configuration: asn1.j2735.kafka.partition = 0
[171011 18:25:55.221425] [info] kafka configuration: metadata.broker.list = 172.17.0.1:9092
[171011 18:25:55.221428] [info] ASN1_Codec configuration: compression.type = none
[171011 18:25:55.221434] [info] kafka partition: 0
[171011 18:25:55.221440] [info] consumed topic: j2735asn1per
[171011 18:25:55.221441] [info] published topic: j2735asn1xer
[171011 18:25:55.221442] [trace] ending configure()
```

# ACM Configuration

The ACM configuration file is a text file with a prescribed format. It can be used to configure Kafka as well as the ACM.
Comments can be added to the configuration file by starting a line with the '#' character. Configuration lines consist
of two strings separated by a '=' character; lines are terminated by newlines. The names of configuration files can be
anything; extensions do not matter.

The following is an example of a portion of a configuration file:

```bash
# Kafka group.
group.id=0

# Kafka topics for ASN.1 Parsing
asn1.topic.consumer=j2735asn1per
asn1.topic.producer=j2735asn1xer

# Amount of time to wait when no message is available (milliseconds)
asn1.consumer.timeout.ms=5000

# For testing purposes, use one partition.
asn1.kafka.partition=0

# The host ip address for the Broker.
metadata.broker.list=localhost:9092

# specify the compression codec for all data generated: none, gzip, snappy, lz4
compression.type=none
```

Example configuration files can be found in the [asn1_codec/config](../config) directory, e.g.,
[example.properties](../config/example.properties) is an example of a complete configuration file.

The details of the settings and how they affect the function of the ACM follow:

## ODE Kafka Interface

- `asn1.topic.producer` : The Kafka topic name where the ACM will write its output. **The name is case sensitive.**

- `asn1.topic.consumer` : The Kafka topic name used by the Operational Data Environment (or other producer) that will be
  consumed by the ACM. **The name is case sensitive.**

- `asn1.consumer.timeout.ms` : The amount of time the consumer blocks (or waits) for a new message. If a message is
  received before this time has elapsed it will be processed immediately.

- `group.id` : The group identifier for the ACM consumer.  Consumers label
  themselves with a consumer group name, and each record published to a topic is
  delivered to one consumer instance within each subscribing consumer group.
  Consumer instances can be in separate processes or on separate machines.

- `asn1.kafka.partition` : The partition(s) consumed by this ACM. A Kafka topic can be divided,
  or partitioned, into several "parallel" streams. A topic may have many partitions so it can handle an arbitrary
  amount of data.

- `metadata.broker.list` : This is the IP address of the Kafka topic broker leader.

- `compression.type` : The type of compression to use for writing to Kafka topics. Currently, this should be set to none.

# ACM Testing with Kafka

There are four steps that need to be started / run as separate processes.

1. Start the `kafka-docker` container to provide the basic kafka data streaming services.

```bash
$ docker-compose up --no-recreate -d
```

1. Start the ACM (here we are starting a decoder).

```bash
$ ./acm -c config/example.properties -T decode
```

1. Use the provided `kafka-console-producer.sh` script (provided with the Apache Kafka installation) to send XML
messages to the ACM. Each time you execute the command below a single message is sent to the ACM.

```bash
$ cat <message> | ./bin/kafka-console-producer.sh --broker-list ${SERVER_IP} --topic ${ACM_INPUT_TOPIC}
```

1. Use the provided `kafka-console-consumer.sh` script (provided with the Apache Kafka installation) to receive XML
messages from the ACM. This process with wait for messages to be published by the ACM.

```bash
$ ./bin/kafka-console-consumer.sh --bootstrap-server ${SERVER_IP} --topic ${ACM_OUTPUT_TOPIC}
```

The log files will provide more information about the details of the processing that is taking place and document any
errors.
