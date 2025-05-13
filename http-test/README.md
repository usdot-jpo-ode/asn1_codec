# HTTP Tests

This folder contains integration tests for the HTTP endpoints.

To use them, run the app in HTTP mode in docker, for example via:

```
docker compose -f docker-compose-server-amazonlinux.yml up --build -d
```

There are two types of tests: *.http files that can be run in an IDE, and shell scripts that can
be run from a command line.

## .http files 

These can be run in the VScode or CLion IDE. In VScode, install the
[REST Client](https://marketplace.visualstudio.com/items?itemName=humao.rest-client) extension.

* [batch-json.http](batch-json.http) - Tests the endpoint for decoding a batch of JSON messages.
* [batch-text-plain.http](batch-text-plain.http) - Tests the endpoint for decoding batch of plain text hex messages.
* [spat-uper-to-xer.http](spat-uper-to-xer.http) - Tests the endpoint for decoding a SPAT UPER hex message to XER.

## .sh scripts

The scripts require a bash command line with curl, such as Git Bash or WSL on Windows.

* [test-large-batch.sh](test-large-batch.sh) - Test decoding a large batch file consisting of SPAT, MAP, BSM, and SSM messages. Outputs result to batch.log file.
* [test-thread-safety.sh](test-thread-safety.sh) - Test thread safety by repeatedly sending large batches of SPAT/MAP/BSM/SSM messages on four simultaneous connections. Outputs results to batch{#}.log file.

