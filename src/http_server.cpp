#include "http_server.hpp"

#include "cpp-httplib/httplib.h"



Http_Server::Http_Server(ASN1_Batch_Codec& asn1_batch_codec) :
    batch(asn1_batch_codec)
{
}

Http_Server::~Http_Server()
{
}

// Does *not" leak, but memory usage plateaus at 4GB under continuous load.
bool Http_Server::http_server() {
    httplib::Server svr;

    svr.Get("/hello", [](const httplib::Request & /*req*/, httplib::Response &res){
        res.set_content("Hello world!", "text/plain");
    });


    svr.Post("/batch/uper/hex/xer", 
        [this](const httplib::Request &req, httplib::Response &res){
            
            std::vector<std::string> hex_line_array;
            std::vector<std::string> xml_line_array;
            std::string hex_line;
            long msgCount = 0;

            std::istringstream infile(req.body);
            // req.body has newlines stripped for unknown reasons, use space as delimiter
            const char delim(' ');
            while (std::getline(infile, hex_line, delim)) {
                if (hex_line == "") break;
                ++msgCount;
                hex_line_array.push_back(hex_line);
            }
            std::cout << "Read " << msgCount << " hex lines" << std::endl;  
            
            bool err = batch.decode_messageframe_data_batch(hex_line_array, xml_line_array);
            
            std::ostringstream outfile;
            for (auto an_xml_line : xml_line_array) {
                outfile << an_xml_line << std::endl;
            }
            std::string xml_result(outfile.str());
            res.set_content(xml_result, "text/plain");
        });
    
    std::cout << "Starting HTTP server" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return EXIT_SUCCESS;
}