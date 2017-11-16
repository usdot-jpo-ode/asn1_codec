#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "acm.hpp"
#include "utilities.hpp"

bool loadTestCases( const std::string& case_file, StrVector& case_data ) {

    std::string line;
    std::ifstream file{ case_file };

    if ( file.good() ) {
        while ( std::getline( file, line ) ) {
            string_utilities::strip( line ); 
            if ( line.length() > 0 && line[0] != '#' ) {
                // skip empty lines and comments.
                case_data.push_back( line );
            }
        }

    } else {
        return false;
    }

    return( !case_data.empty() );
}

pugi::xpath_query ode_payload_query("OdeAsn1Data/payload/data");
pugi::xml_document output_doc;
pugi::xml_parse_result parse_result;
pugi::xml_node payload_node;
pugi::xml_node byte_node;
ASN1_Codec asn1_codec{"ASN1_Codec","ASN1 Processing Module"};

TEST_CASE("ASN1_Codex Tests", "[encoding]" ) {
    const char *BSM_HEX = "001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
    const char *ASD_BSM_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F1620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";
    const char *ONE609_BSM_HEX = "038081B1001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
    const char *ASD_ONE609_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F16A070103620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";

    //<payload><data><AdvisorySituationData><bytes>

    std::stringstream out1;
    CHECK(asn1_codec.file_test("unit-test-data/BSM.xml", out1) == EXIT_SUCCESS);
    parse_result = output_doc.load(out1, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("MessageFrame").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);

    
    std::stringstream out2;
    CHECK(asn1_codec.file_test("unit-test-data/ASD.xml", out2) == EXIT_SUCCESS);
    parse_result = output_doc.load(out2, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);


    std::stringstream out3;
    CHECK(asn1_codec.file_test("unit-test-data/ASD_BSM.xml", out3) == EXIT_SUCCESS);
    parse_result = output_doc.load(out3, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);
    byte_node = payload_node.child("MessageFrame").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);


    std::stringstream out4;
    CHECK(asn1_codec.file_test("unit-test-data/1609.xml", out4) == EXIT_SUCCESS);
    parse_result = output_doc.load(out4, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);


    std::stringstream out5;
    CHECK(asn1_codec.file_test("unit-test-data/ASD_1609.xml", out5) == EXIT_SUCCESS);
    parse_result = output_doc.load(out5, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_ONE609_HEX) == 0);


    std::stringstream out6;
    CHECK(asn1_codec.file_test("unit-test-data/1609_BSM.xml", out6) == EXIT_SUCCESS);
    parse_result = output_doc.load(out6, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
    byte_node = payload_node.child("MessageFrame").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);

    std::stringstream out7;
    CHECK(asn1_codec.file_test("unit-test-data/ASD_1609_BSM.xml", out7) == EXIT_SUCCESS);
    parse_result = output_doc.load(out7, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
    byte_node = payload_node.child("MessageFrame").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_ONE609_HEX) == 0);

    // TODO check oracles with decoder
}
