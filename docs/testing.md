# Testing the ACM

There is currently way to test the capabilities of the ACM, aside from deployment.
- [Unit Testing](#unit-testing)

## Test Files

Several example JSON message test files are in the [asn1_codec/data](../data) directory.  These files can be edited to generate
your own test cases. Each line in the file should be a well-formed BSM or TIM JSON
object. **Each message should be on a separate line in the file.** **If a JSON object cannot be parsed it is suppressed.**

## Unit Testing

Unit tests are built when the ACM is compiled during installation. Those tests can be run using the following command:

```bash
$ ./acm_tests
```
