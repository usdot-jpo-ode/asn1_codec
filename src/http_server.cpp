#include "http_server.hpp"
#include "acmLogger.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using namespace std::chrono;
using namespace nlohmann;
using namespace nlohmann::json_abi_v3_11_3;

Http_Server::Http_Server(ASN1_Codec& asn1_codec) :
    codec(asn1_codec),
    logger("http_server")
{
    string portString = getEnvironmentVariable("ACM_HTTP_SERVER_PORT");
    string concurrencyString = getEnvironmentVariable("ACM_HTTP_SERVER_CONCURRENCY");

    if (!portString.empty()) {
        port = stoi(portString);
    } else {
        ostringstream msg;
        msg << "WARNING: ACM_HTTP_SERVER_PORT env variable is not set, using default: " << port;
        logger.warn(msg.str());
    }

    if (!concurrencyString.empty()) {
        concurrency = stoi(concurrencyString);
    } else {
        ostringstream msg;
        msg << "WARNING: ACM_HTTP_SERVER_CONCURRENCY env variable is not set, using default: " << concurrency;
        logger.warn(msg.str());
    }
}

Http_Server::~Http_Server()
{
}

long Http_Server::get_epoch_milliseconds() {
    const auto t = system_clock::now();
    const auto epoch = t.time_since_epoch();
    long millis = duration_cast<milliseconds>(epoch).count();
    return millis;
}

const char* Http_Server::getEnvironmentVariable(std::string variableName) {
    char* variableValue = getenv(variableName.c_str());
    if (variableValue == NULL) {
        return "";
    }
    return variableValue;
}

/**
 * Runs an HTTP server with REST methods of the form:
 *     
 *     /<spec>/<from-encoding>/<to-encoding> 
 * 
 * for single message conversions, or:
 * 
 *     /batch/<spec>/<from-encoding>/<to-encoding> 
 * 
 * for batch conversions.
 * 
 * Path parameters:
 *     
 *     <spec> 
 *         is the name of an ASN.1 specification, eg. j2735, 1609.2, asd
 * 
 *     <to-encoding> and 
 *     <from-encoding> 
 *          are ASN.1 encodings, eg. uper, oer, xer, jer.
 * 
 * Currently supported methods:
 * 
 *     /j2735/uper/xer
 *     /batch/j2735/uper/xer
 *     
 */
bool Http_Server::http_server() {
    
    crow::SimpleApp app;

    /**
     * Endpoint to decode a single UPER/hex J2735 MessageFrame to XER.
     * 
     * Accepts Content-Type:
     * 
     *    text/plain
     * 
     * POST Body:
     * 
     *    One UPER/hex encoded MessageFrame
     * 
     * Returns:
     * 
     *    The MessageFrame converted to XER
     * 
     */
    CROW_ROUTE(app, "/j2735/uper/xer")
        .methods("POST"_method)
        ([this](const crow::request& req) {
            return post_single(req);
        });

    /**
     * Endpoint to decode a batch of UPER/hex J2735 MessageFrames to XER.
     * 
     * Accepts Content-Types:
     *     
     *   text/plain,
     *   application/x-ndjson, or other json types.
     * 
     * POST Body: 
     * 
     *   Either plain text containing one hex MessageFrame per line:
     * 
     *     001355222...
     *     0014250f0...
     * 
     *   or line-delimited JSON containing one JSON message per line of the form:
     * 
     *     { "timestamp": 1683155399091, "type": "SPAT", "hex": "0013..." }
     *     { "timestamp": 1683155410467, "type": "BSM",  "hex": "0014..."  }
     *     ...
     * 
     * Returns:
     * 
     *   For plain text input, returns line-delimited XER:
     * 
     *     <MessageFrame><messageId>19</messageId><value><SPAT>...
     *     <MessageFrame><messageId>20</messageId><value><BasicSafetyMessage>...
     * 
     *   For JSON input, returns line-delimited XER alternating with the metadata in the format:
     * 
     *     SPAT,1683155399091
     *     <MessageFrame><messageId>19</messageId><value><SPAT>...
     *     BSM,1683155410467
     *     <MessageFrame><messageId>20</messageId><value><BasicSafetyMessage>...
     */
    CROW_ROUTE(app, "/batch/j2735/uper/xer")
        .methods("POST"_method)
        ([this](const crow::request& req){
            return post_batch(req);
        });


    ostringstream msg;
    msg << "Starting HTTP server on port " << port << ", using " << concurrency << " threads.";
    logger.info(msg.str());

    app.port(port)
        .concurrency(concurrency)
        .loglevel(crow::LogLevel::Warning)
        .run();

    return EXIT_SUCCESS;
}

crow::response Http_Server::post_single(const crow::request &req) {
    string hex_line(req.body);
    buffer_structure_t xb = {0, 0, 0};
    bool decodeOk = codec.decode_messageframe_data(hex_line, &xb);
    if (!decodeOk) {
        free(xb.buffer);
        string msg("Error decoding uper: ");
        string err_msg = msg + hex_line;
        logger.error(err_msg);
        return crow::response(400, "text/plain", err_msg);
    }
    string xml_line(xb.buffer, xb.buffer_size);
    free(xb.buffer);
    return crow::response("application/xml", xml_line);
}

crow::response Http_Server::post_batch(const crow::request &req) {
    string content_type = req.get_header_value("Content-Type");
    const bool is_json = content_type.find("json") != string::npos;
    {
        ostringstream msg;
        msg << "Content-Type: " << content_type << ", is json: " << is_json;
        logger.info(msg.str());
    }
    long t1millis = get_epoch_milliseconds();
    {
        ostringstream msg;
        msg << "Start decoding at " << t1millis;
        logger.info(msg.str());
    }

    istringstream infile(req.body);
    ostringstream outfile;
    long msgCount = 0;
    string line;
    string hex_line;
    json json_value;
    long timestamp;
    string message_type;

    while (getline(infile, line)) {
        try {
            if (line == "") continue;
            ++msgCount;

            if (is_json) {
                try {
                    json_value = json::parse(line);
                    timestamp = json_value["timestamp"];
                    message_type = json_value["type"];
                    hex_line = json_value["hex"];
                } catch (json::parse_error& ex) {
                    logger.error("json parse error in line " + line);
                    continue;
                }
            } else {
                hex_line = line;
            }

            buffer_structure_t xb = {0, 0, 0};
            bool decodeOk = codec.decode_messageframe_data(hex_line, &xb);
            if (!decodeOk) {
                free(xb.buffer);
                logger.error("Error decoding uper: " + hex_line);
                continue;
            }
            string xml_line(xb.buffer, xb.buffer_size);
            free(xb.buffer);

            // If json, write additional info on line before decoded xml
            if (is_json) {
                outfile << message_type << "," << timestamp << endl;
            }

            outfile << xml_line << endl;
        } catch (exception& ex) {
            logger.error(ex.what());
        }
    }

    long t2millis = get_epoch_milliseconds();
    long delta = t2millis - t1millis;
    {
        ostringstream msg;
        msg << "Finished decoding " << msgCount << " uper messages in " << delta << " milliseconds.";
        logger.info(msg.str());
    }

    string xml_result(outfile.str());
    return crow::response("text/plain", xml_result);
}

