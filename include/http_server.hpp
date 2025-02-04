#ifndef ACM_HTTP_SERVER_H
#define ACM_HTTP_SERVER_H

#include "acm.hpp"
#include "acmLogger.hpp"

class Http_Server {
    public:
        Http_Server(ASN1_Codec& asn1_codec);
        ~Http_Server();
        bool http_server();
    private:
        ASN1_Codec& codec;
        AcmLogger logger;
        long get_epoch_milliseconds();
};

#endif