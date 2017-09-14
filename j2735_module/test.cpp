#include "asn1-j2735-lib.h"
#include<iostream>
#include<cstdio>
#include<sstream>

int main( int argc, char** argv ) {

    // looks like if this buffer size is too small then an entire record will not be processed and things will FAIL!
    static const std::size_t BUFSIZE = 512;
	uint8_t buf[BUFSIZE];

    long bytes_read{0};
    long total_bytes{0};

    // the decoded result.
    void* structure;

    std::string ifn{ argv[1] };

    FILE* source = fopen( ifn.c_str(), "rb" );
//    FILE* dest   = fopen( ofn.c_str(), "rb" );

    if ( !source ) {
        std::cerr << "Cannot open the source file.\n";
        std::exit( EXIT_FAILURE );
    }

//    if ( !dest ) {
//        std::cerr << "Cannot open the destination file.\n";
//        std::exit( EXIT_FAILURE );
//    }

    // JMC: In our case this points to the asn_TYPE_descriptor_t asn_DEF_MessageFrame structure defined in
    // MessageFrame.c. The asn_TYPE_descriptor_t is in constr_TYPE.h
    static asn_TYPE_descriptor_t *pduType = &PDU_Type;

	// input file format important!!
	iform = INP_PER;

    // pduType->op is a structure of function pointers that perform operations on the ASN.1.

    //std::size_t suggested_bufsize = 8*2^10;
    ssize_t suggested_bufsize = 8*2^10;
    bool first_block = true;

	while ( !feof( source ) ) {

        // attempt to read 1K bytes into buffer.
        bytes_read = fread( buf, sizeof buf[0], BUFSIZE, source );
        total_bytes += bytes_read;

        structure = data_decode_from_buffer(pduType, buf, bytes_read, first_block);
        if(!structure) {
            std::cerr << "No structure returned from decoding.\n";
            std::exit( EXIT_FAILURE );
        }

        // JMC: Dump these to a string stream instead or directly to the kafka buffer.
        // fprintf( stdout, "JMC: OUT_XER\n" );
        if(xer_fprint(stdout, pduType, structure)) {
            std::cerr << ifn << ": Cannot convert " << pduType->name << " into XML\n";
            std::exit( EXIT_FAILURE );
        }
        std::cout << "**** BREAK ****\n";
        first_block = false;
    }

    std::exit( EXIT_SUCCESS );
}
