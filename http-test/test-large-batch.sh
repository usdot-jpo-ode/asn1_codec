#!/bin/bash

curl --header "Content-Type: text/plain" --data-binary "@batch.data.hex" http://localhost:9999/batch/j2735/uper/xer > batch.log