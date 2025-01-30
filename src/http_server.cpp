#include "http_server.hpp"

// Note: Crow depends on asio.  Asio standalone library does not compile
// on Alpine due to not being able to find <linux/version.h> header
#include "crow/crow_all.h"

using namespace std;

Http_Server::Http_Server(ASN1_Batch_Codec& asn1_batch_codec) :
    batch(asn1_batch_codec)
{
}

Http_Server::~Http_Server()
{
}


bool Http_Server::http_server() {
    // httplib::Server svr;

    // svr.Get("/hello", [](const httplib::Request & /*req*/, httplib::Response &res){
    //     res.set_content("Hello world!", "text/plain");
    // });


    // svr.Post("/batch/uper/hex/xer", 
    //     [this](const httplib::Request &req, httplib::Response &res){
            
    //         std::vector<std::string> hex_line_array;
    //         std::vector<std::string> xml_line_array;
    //         std::string hex_line;
    //         long msgCount = 0;

    //         std::istringstream infile(req.body);
    //         // req.body has newlines stripped for unknown reasons, use space as delimiter
    //         const char delim(' ');
    //         while (std::getline(infile, hex_line, delim)) {
    //             if (hex_line == "") break;
    //             ++msgCount;
    //             hex_line_array.push_back(hex_line);
    //         }
    //         std::cout << "Read " << msgCount << " hex lines" << std::endl;  
            
    //         bool err = batch.decode_messageframe_data_batch(hex_line_array, xml_line_array);
            
    //         std::ostringstream outfile;
    //         for (auto an_xml_line : xml_line_array) {
    //             outfile << an_xml_line << std::endl;
    //         }
    //         std::string xml_result(outfile.str());
    //         res.set_content(xml_result, "text/plain");
    //     });
    
    // std::cout << "Starting HTTP server" << std::endl;
    // svr.listen("0.0.0.0", 8080);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/hello")
    ([]() {
        return "Hello world, this is crow!!";
    });

    // From https://github.com/CrowCpp/Crow/blob/master/examples/example.cpp#L153
    //      * curl -d '{"a":1,"b":2}' {ip}:18080/add_json
    CROW_ROUTE(app, "/add_json")
      .methods("POST"_method)([](const crow::request& req) {
          auto x = crow::json::load(req.body);
          if (!x)
              return crow::response(400);
          int sum = x["a"].i() + x["b"].i();
          ostringstream os;
          os << sum;
          return crow::response{os.str()};
      });

    CROW_ROUTE(app, "/batch/uper/hex/xer")
        .methods("POST"_method)([&](const crow::request& req){
            vector<string> hex_line_array;
            vector<string> xml_line_array;
            string hex_line;
            long msgCount = 0;
            
            istringstream infile(req.body);

            while (getline(infile, hex_line)) {
                if (hex_line == "") break;
                ++msgCount;
                hex_line_array.push_back(hex_line);
            }
            cout << "Read " << msgCount << " hex lines" << endl;  
            
            bool err = batch.decode_messageframe_data_batch(hex_line_array, xml_line_array);
            
            std::ostringstream outfile;
            for (auto an_xml_line : xml_line_array) {
                outfile << an_xml_line << endl;
            }
            string xml_result(outfile.str());
            
            return crow::response("text/plain", xml_result);
        });

    cout << "Starting HTTP server" << endl;
    app.port(8080).run();

    return EXIT_SUCCESS;
}