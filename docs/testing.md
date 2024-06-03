# Testing the ACM

There are currently two ways to test the capabilities of the ACM.
- [Unit Testing](#unit-testing)
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