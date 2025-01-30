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

    CROW_ROUTE(app, "/batch/uper/<string>/xer")
        .methods("POST"_method)
        ([this](const crow::request& req, string uper_text_encoding){

            cout << uper_text_encoding << endl;

            if (uper_text_encoding != "hex" && uper_text_encoding != "b64") {
                cerr << "Unknown uper text encoding: " << uper_text_encoding 
                    << ". 'hex' or 'b64' are allowed." << endl;
                return crow::response(400, "text/plain", "Error");
            }

            vector<string> hex_line_array;
            vector<string> xml_line_array;
            string hex_line;
            long msgCount = 0;
            
            istringstream infile(req.body);

            // Use curl --data-binary to preserve newlinse
            while (getline(infile, hex_line)) {
                if (hex_line == "") break;
                ++msgCount;
                hex_line_array.push_back(hex_line);
            }
            cout << "Read " << msgCount << " uper lines" << endl;  
            
            vector<vector<char>> bytes = batch.batch_hex_to_bytes(hex_line_array);
            bool err2 = batch.decode_messageframe_data_batch(bytes, xml_line_array);
            
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