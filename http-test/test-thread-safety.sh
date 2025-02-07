#!/bin/bash

# Stress test to check for thread safety
#
# Prerequisites:
#   * Docker
#   * A Bash command line (for example Git Bash or WSL on Windows)
#   * curl installed
#
# This script sends large batches of message frames over 4 simultaneous HTTP connections.
#
# Run the app in server mode in Docker, and observe the container log for decoding errors
# while running this script.  If intermittent/periodic decoding errors are seen with this
# script, but a single threaded query does not cause errors, it indicates a problem with
# thread safety.



function send_batch {
    for run in {1..20}; do
        curl --header "Content-Type: text/plain" --data-binary "@batch.data.hex" http://localhost:9999/batch/j2735/uper/xer > $1;
    done
}

echo "thread 1: start sending batches"
send_batch "batch1.log" &
echo "thread 2: start sending batches"
send_batch "batch2.log" &
echo "thread 3: start sending batches"
send_batch "batch3.log" &
echo "thread 4: start sending batches"
send_batch "batch4.log" &

