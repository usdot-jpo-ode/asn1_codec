#ifndef ACM_BATCH_H
#define ACM_BATCH_H

#include "acm.hpp"

class ASN1_Batch_Codec {
    public:
        ASN1_Batch_Codec(ASN1_Codec& asn1_codec);
        ~ASN1_Batch_Codec();
        bool batch(std::string input_file, std::string output_file);
    private:
        ASN1_Codec& codec;
        long get_epoch_milliseconds();

};

#endif