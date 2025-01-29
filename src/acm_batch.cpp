#include "acm_batch.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

ASN1_Batch_Codec::ASN1_Batch_Codec(ASN1_Codec& asn1_codec) : codec(asn1_codec)
{
}

ASN1_Batch_Codec::~ASN1_Batch_Codec()
{
}

long ASN1_Batch_Codec::get_epoch_milliseconds() {
    const auto t = std::chrono::system_clock::now();
    const auto epoch = t.time_since_epoch();
    long millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    return millis;
}

bool ASN1_Batch_Codec::batch(std::string input_file, std::string output_file) 
{
    
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "Output file: " << output_file << std::endl;

    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outfile(output_file);
    if (outfile.fail()) {
        std::cerr << "Error opening output file." << std::endl;
        return EXIT_FAILURE;
    }

    std::string hex_line;
    long msgCount = 0;
    bool err;

    long t1millis = get_epoch_milliseconds();
    std::cout << "Start decoding at " << t1millis << std::endl;
    
    while (std::getline(infile, hex_line)) {
        if (hex_line == "") break;
        ++msgCount;
        buffer_structure_t xb = {0, 0, 0};
        err = codec.decode_messageframe_data(hex_line, &xb);
        std::string xml_line(xb.buffer, xb.buffer_size);
        std::free(static_cast<void *>(xb.buffer));
        outfile << xml_line << std::endl;
    }

    long t2millis = get_epoch_milliseconds();
    long delta = t2millis - t1millis;

    infile.close();
    outfile.close();
     
    std::cout << "Finished decoding " << msgCount << " lines in " << delta << " milliseconds." <<  std::endl;
    std::cout << "Wrote xml to output file " << output_file << std::endl;

    return err ? EXIT_SUCCESS : EXIT_FAILURE;
}

