#!/usr/bin/env python3

from __future__ import print_function
import xml.etree.ElementTree as elementTree

import sys

exceptionOccurred = False

for line in sys.stdin:
    try:
        rootElement = elementTree.fromstring(line.strip())

        print('Encodings', file=sys.stderr)

        for encoding in rootElement.findall('./metadata/encodings/encodings'):
            print('  Name: {}'.format(encoding[0].text), file=sys.stderr)
            print('  Type: {}'.format(encoding[2].text), file=sys.stderr)
        
    except Exception as e:
        print("Exception occurred in 'test_in.py'...")
        exceptionOccurred = True
        continue

    sys.stdout.write(line)

if exceptionOccurred:
    sys.exit(1)