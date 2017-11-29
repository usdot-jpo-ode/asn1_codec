#!/usr/bin/env python3

from __future__ import print_function
import xml.etree.ElementTree as et

import sys

for l in sys.stdin:
    try:
        root = et.fromstring(l.strip())

        print('Encodings', file=sys.stderr)

        for encoding in root.findall('./metadata/encodings/encodings'):
            print('  Name: {}'.format(encoding[0].text), file=sys.stderr)
            print('  Type: {}'.format(encoding[2].text), file=sys.stderr)
        
    except Exception as e:
        continue

    sys.stdout.write(l)
