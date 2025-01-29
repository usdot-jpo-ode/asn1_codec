#ifndef ACM_BATCH_H
#define ACM_BATCH_H

#include "acm.hpp"



class ASN1_Batch_Codec {
    public:
        ASN1_Batch_Codec(ASN1_Codec& asn1_codec);
        ~ASN1_Batch_Codec();
        bool decode_batch_file(std::string input_file, std::string output_file);
        bool decode_messageframe_data_batch(std::vector<std::string>& batch_hex, std::vector<std::string>& batch_xml);
        bool http_server();
    private:
        ASN1_Codec& codec;
        long get_epoch_milliseconds();
};

#endif