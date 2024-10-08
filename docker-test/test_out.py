#!/usr/bin/env python3

# This script reads XML data from stdin, prints the payload data to stdout, and writes any exceptions to stderr.

# This script is used by `do_test.sh` to process the output data and pipe it during data processing.

from __future__ import print_function
import xml.etree.ElementTree as elementTree

import sys

exceptionOccurred = False

for l in sys.stdin:
    try:
        rootElement = elementTree.fromstring(l.strip())

        print('****PayloadData****')

        data = list(rootElement.findall('./payload/data'))[0]

        for frame in data:
            name = frame.tag

            if frame[0].tag == 'bytes':
                print('**Type: {} bytes**'.format(name))
                print(frame[0].text)
            else:
                print('**Type: {} XML**'.format(name))
                print(elementTree.tostring(frame))

            print('****')

    except Exception as e:
        print("Exception occurred in 'test_out.py'...")
        exceptionOccurred = True
        continue

if exceptionOccurred:
    sys.exit(1)