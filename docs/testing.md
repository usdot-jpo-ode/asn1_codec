# Testing the ACM

There are currently three ways to test the capabilities of the ACM.
- [Unit Testing](#unit-testing)
- [Kafka Test Script](#kafka-test-script)
- [Standalone Operation / Testing](#standalone-testing)

## Unit Testing
### Testing On Local Machine
The following command can be run to build & run the unit tests:

```bash
$ ./build_local.sh
```


### Testing Using Docker
Start by building the Docker image:

```bash
$ docker build -t acm .
```

Then run the Docker container:

```bash
$ docker run -it --name acm acm
```

Exec into the container and run the tests:

```bash
$ docker exec -it acm /bin/bash
$ cd /build
$ ./acm_tests
```

## Kafka Test Script
The [do_kafka_test.sh](../do_kafka_test.sh) script is designed to perform integration tests on a Kafka instance. To execute the tests, this script relies on the following scripts: standalone.sh, do_test.sh, test_in.py, and test_out.py

To ensure proper execution, it is recommended to run this script outside of the dev container where docker is available. This is because the script will spin up a standalone kafka instance and will not be able to access the docker daemon from within the dev container.

It should be noted that this script and any dependent scripts need to use the LF end-of-line sequence. These include the following:
- do_kafka_test.sh
- standalone.sh
- do_test.sh
- test_in.py
- test_out.py

The DOCKER_HOST_IP environment variable must be set to the IP address of the host machine. This is required for the script to function properly. This can be set by using the following command:

```
export DOCKER_HOST_IP=$(ifconfig | zgrep -m 1 -oP '(?<=inet\s)\d+(\.\d+){3}')
```

If not set, the script will attempt to resolve the IP address and will exit if it is unable to do so.

## Standalone Operation / Testing

If the `-F` option is used, the ACM will assume its first operand is a filename, attempt to open that file, and decode or
encode it depending on the `-T` option. The ACM's output is written to stdout. The input XML file must not include any
newlines. The following is an example command:

```bash
$ ./acm -F -c config/example.properties -T decode ../data/InputData.Ieee1609Dot2Data.packed.xml
```

### Test Files

Several example JSON message test files are in the [asn1_codec/data](../data) directory.  These files can be edited to generate
your own test cases.