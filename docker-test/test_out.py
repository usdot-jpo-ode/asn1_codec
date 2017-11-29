#!/usr/bin/env python3

from __future__ import print_function
import xml.etree.ElementTree as et

import sys

for l in sys.stdin:
    try:
        root = et.fromstring(l.strip())

        print('****PayloadData****')

        data = list(root.findall('./payload/data'))[0]

        for frame in data:
            name = frame.tag

            if frame[0].tag == 'bytes':
                print('**Type: {} bytes**'.format(name))
                print(frame[0].text)
            else:
                print('**Type: {} XML**'.format(name))
                print(et.tostring(frame))

            print('****')

    except Exception as e:
        continue
