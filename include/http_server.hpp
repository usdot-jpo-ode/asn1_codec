#ifndef ACM_HTTP_SERVER_H
#define ACM_HTTP_SERVER_H

#include "acm.hpp"
#include "acmLogger.hpp"
#include "crow/crow_all.h"

class Http_Server {
    public:
        Http_Server(ASN1_Codec& asn1_codec);
        ~Http_Server();
        bool http_server();
        crow::response post_single(const crow::request& req);
        crow::response post_batch(const crow::request& req);
    private:
        ASN1_Codec& codec;
        AcmLogger logger;
        static const char* getEnvironmentVariable(std::string var);
        static long get_epoch_milliseconds();
        int port = 9999;
        int concurrency = 4;
};

#endif