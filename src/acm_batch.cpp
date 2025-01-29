#include "acm_batch.hpp"

#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace nlohmann;
using namespace nlohmann::json_abi_v3_11_3;
using namespace std::chrono;
using namespace std;

ASN1_Batch_Codec::ASN1_Batch_Codec(ASN1_Codec& asn1_codec) : codec(asn1_codec)
{
}

ASN1_Batch_Codec::~ASN1_Batch_Codec()
{
}

long ASN1_Batch_Codec::get_epoch_milliseconds() {
    const auto t = system_clock::now();
    const auto epoch = t.time_since_epoch();
    long millis = duration_cast<milliseconds>(epoch).count();
    return millis;
}

bool ASN1_Batch_Codec::decode_batch_file(string input_file, string output_file) 
{
    
    cout << "Input file: " << input_file << endl;
    cout << "Output file: " << output_file << endl;

    ifstream infile(input_file);
    if (!infile.is_open()) {
        cerr << "Error opening input file." << endl;
        return EXIT_FAILURE;
    }

    ofstream outfile(output_file);
    if (outfile.fail()) {
        cerr << "Error opening output file." << endl;
        return EXIT_FAILURE;
    }

    json json_value;
    string json_line;
    string hex_line;
    long timestamp;
    string message_type;
    long msgCount = 0;
    bool err;

    long t1millis = get_epoch_milliseconds();
    cout << "Start decoding at " << t1millis << endl;
    
    while (getline(infile, json_line)) {
        if (json_line == "") break;
        ++msgCount;
        
        try {
            json_value = json::parse(json_line);
            timestamp = json_value["timestamp"];
            message_type = json_value["type"];
            hex_line = json_value["hex"];
        } catch (json::parse_error& ex) {
            err = false;
            cout << "json parse error in line " << json_line << endl;
            continue;
        }

        buffer_structure_t xb = {0, 0, 0};
        err = codec.decode_messageframe_data(hex_line, &xb);
        string xml_line(xb.buffer, xb.buffer_size);
        free(static_cast<void *>(xb.buffer));
        outfile << message_type << "," << timestamp << endl;
        outfile << xml_line << endl;
    }

    long t2millis = get_epoch_milliseconds();
    long delta = t2millis - t1millis;

    infile.close();
    outfile.close();
     
    cout << "Finished decoding " << msgCount << " lines in " << delta << " milliseconds." <<  endl;
    cout << "Wrote xml to output file " << output_file << endl;

    return err ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool ASN1_Batch_Codec::decode_messageframe_data_batch(vector<string>& batch_hex, vector<string>& batch_xml) {
    bool err;

    long t1millis = get_epoch_milliseconds();
    cout << "Start decoding at " << t1millis << endl;
    
    buffer_structure_t xb = {0, 0, 0};
    for (auto a_hex_line : batch_hex) {
        xb.buffer = 0;
        xb.buffer_size = 0;
        xb.allocated_size = 0;
        err = codec.decode_messageframe_data(a_hex_line, &xb);
        string xml_line(xb.buffer, xb.buffer_size);
        free(static_cast<void *>(xb.buffer));
        batch_xml.push_back(xml_line);
    }

    long t2millis = get_epoch_milliseconds();
    long delta = t2millis - t1millis;
    cout << "Finished decoding in " << delta << " milliseconds." <<  endl;

    return err;
}

