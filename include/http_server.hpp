#ifndef ACM_HTTP_SERVER_H
#define ACM_HTTP_SERVER_H

#include "acm.hpp"
#include "acm_batch.hpp"

class Http_Server {
    public:
        Http_Server(ASN1_Batch_Codec& batch_codec);
        ~Http_Server();
        bool http_server();
    private:
        ASN1_Batch_Codec& batch;
};

#endif