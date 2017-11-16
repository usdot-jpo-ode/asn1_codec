# Testing the ACM

There is currently two ways to test the capabilities of the ACM.
- [Unit Testing](#unit-testing)
- [Standalone Operation / Testing](#standalone-testing)

## Test Files

Several example JSON message test files are in the [asn1_codec/data](../data) directory.  These files can be edited to generate
your own test cases.

## Standalone Operation / Testing

If the `-F` option is used, the ACM will assume its first operand is a filename, attempt to open that file, and decode or
encode it depending on the `-T` option. The ACM's output is written to stdout. The input XML file must not include any
newlines. The following is an example command:

```bash
$ ./acm -F -c config/example.properties -T decode ../data/InputData.Ieee1609Dot2Data.packed.xml
```

## Unit Testing

Unit tests are built when the ACM is compiled during installation. Those tests can be run using the following command:

```bash
$ ./acm_tests
```
