#include "pugixml.hpp"
#include "asn1-j2735-lib.h"

#include <iostream>

int decode_hex(const std::string& payload_hex, char *payload_bytes) {
    size_t i = 0;

    for (const char& c : payload_hex) {
        switch(c) {
            case '0':
                payload_bytes[i] = 0x0;
                
                break;
            case '1':
                payload_bytes[i] = 0x1;
                
                break;
            case '2':
                payload_bytes[i] = 0x2;
                
                break;
            case '3':
                payload_bytes[i] = 0x3;
                
                break;
            case '4':
                payload_bytes[i] = 0x4;
                
                break;
            case '5':
                payload_bytes[i] = 0x5;
                
                break;
            case '6':
                payload_bytes[i] = 0x6;
                
                break;
            case '7':
                payload_bytes[i] = 0x7;
                
                break;
            case '8':
                payload_bytes[i] = 0x8;
                
                break;
            case '9':
                payload_bytes[i] = 0x9;
                
                break;
            case 'a':
                payload_bytes[i] = 0xa;
                
                break;
            case 'b':
                payload_bytes[i] = 0xb;
                
                break;
            case 'c':
                payload_bytes[i] = 0xc;
                
                break;
            case 'd':
                payload_bytes[i] = 0xd;
                
                break;
            case 'e':
                payload_bytes[i] = 0xe;
                
                break;
            case 'f':
                payload_bytes[i] = 0xf;
                
                break;
            default:
                return -1;
        }

        ++i;
    }

    return 0;
}

int main()
{
    char payload_bytes[256];

    pugi::xml_document doc;
    pugi::xml_document asn1_doc;
    

    // TODO check for missing files and missing nodes
    // TODO do we need to validate?
    pugi::xml_parse_result result = doc.load_file("asn1c_input.xml");
    pugi::xml_node payload = doc.child("root").child("payload");
    
    std::string hex_str = payload.child_value("data");


    if (!payload.remove_child("data")) {
        // TODO removal error
    }

    // Convert to bytes.
    if (decode_hex(hex_str, payload_bytes)) {
        // TODO decode error 
        std::cerr << "err" << std::endl;
    }

    pugi::xml_node payload_data = payload.append_child("data");

    /*
    for (int i = 0; i < hex_str.length(); ++i) {
        std::cerr << std::hex << (int) payload_bytes[i];
    }
    std::cerr << std::endl;
    */

    static asn_TYPE_descriptor_t *pduType = &PDU_Type;
    void *structure = data_decode_from_buffer(pduType, (const uint8_t *) payload_bytes, hex_str.length(), false);

    result = asn1_doc.load_buffer((const void*) structure, hex_str.length());

    asn1_doc.save(std::cout);

    // TODO set the data from asn1 here
    payload_data.text().set("foo");
}
