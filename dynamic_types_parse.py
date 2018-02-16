#!/usr/bin/env python

import os
import csv
import sys
import re

def main( infile, outfile ):

    recomment = re.compile(r"\/\*.*\*\/", re.IGNORECASE)
    restructure = re.compile(r"{.*},", re.IGNORECASE)
    retype = re.compile(r"asn_TYPE_descriptor_t (asn_DEF_.+).*=.*{.*", re.IGNORECASE)
    restring = re.compile(r"\"(.*)\",", re.IGNORECASE)
    reend = re.compile(r"};", re.IGNORECASE)

    buf = ""
    data = []
    start = True
    for line in infile:
        line = line.strip()

        if ( len(line) == 0 ):
            continue

        if ( line == "--" ):
            # start new record.
            start = True
            continue

        elif ( start ):
            # string out comment if it exists.
            line = recomment.sub("",line).strip()

            # end of the dynamic type structure.
            match = reend.match( line );
            if ( match ):
                if ( len(buf)>0 ):
                    data[-1].append( buf );
                start = False
                buf = ""
                continue

            # the dynamic type definition with the name; this is the start of a new record.
            match = retype.match( line )
            if ( match ):
                data.append([])
                buf = ""
                # grab just the type name in the code.
                data[-1].append( match.group(1) )
                continue

            # sometimes there is a structure instantiated, grab that the commas screw up simple comma based splits.
            match = restructure.match( line )
            if ( match ):
                if ( len(buf)>0 ):
                    data[-1].append( buf + match.group(0)[:-1] )
                    buf = ""
                else:
                    data[-1].append( match.group(0)[:-1] )
                continue

            # The strings that will be put in the hash map; one is the name, one is the XML tag.
            match = restring.match( line )
            if ( match ):
                if ( len(buf)>0 ):
                    data[-1].append( buf + match.group(1) )
                    buf = ""
                else:
                    data[-1].append( match.group(1) )
                continue
                
            # all the specific cases out of the way, just search for lines ending in comma.
            if ( line[-1] == "," ):
                # commas signal end of field in record; could be multiple fields (intra field commas).
                parts = line.split(",")
                i = 0
                for p in parts:
                    p = p.strip()
                    if ( i == 0 ):
                        if ( len(buf)>0 ):
                            data[-1].append( buf + p )
                            buf = ""
                        else:
                            data[-1].append( p )
                    else:
                        # add fields with size
                        if ( len(p)>0 ):
                            data[-1].append( p )
                    i += 1
                continue

            # nothing matched so this part append to the previous.
            buf = buf + line

    # print out the structures.
    i = 1;
    for record in data:
        # print('line: {} length: {}'.format(i, len(record)))
        outfile.write( ":".join( str(x) for x in record ) )
        outfile.write( '\n' )
        i+=1

if __name__ == "__main__":

    if ( len(sys.argv) == 1 ):
        # for pipelining.
        main( sys.stdin, sys.stdout )

    elif ( len(sys.argv) == 2 ):
        # pipe in and specific file out.
        outfile = open( sys.argv[1], "w+" )
        main( sys.stdin, outfile )
        outfile.close()

    elif ( len(sys.argv) == 3 ):
        # file in and file out.
        infile = open( sys.argv[1], "r" )
        outfile = open( sys.argv[2], "w+" )
        main( infile, outfile )
        outfile.close()
        infile.close()

    else:
        print('read the code.')

