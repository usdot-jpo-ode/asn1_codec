#define CATCH_CONFIG_MAIN
#include <http_server.hpp>
#include <crow/crow_all.h>

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
pugi::xpath_query bsm_vehicle_event_flags_query("MessageFrame/value/BasicSafetyMessage/partII/BSMpartIIExtension/partII-Value/VehicleSafetyExtensions/events");
pugi::xml_document output_doc;
pugi::xml_parse_result parse_result;
pugi::xml_node payload_node;
pugi::xml_node byte_node;
ASN1_Codec asn1_codec{"ASN1_Codec","ASN1 Processing Module"};

const char *BSM_HEX = "001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
const char *ASD_BSM_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F1620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";
const char *ONE609_BSM_HEX = "038081B1001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
const char *ASD_ONE609_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F16A070103620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";

const char* BATCH_HEX = R"(0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d86a8083000821a087c087c000811503ff43ff400608601e4100604540ffd0ffd004021a089dc8ba802410c03c8201408601e41
00136d2222e3dfbb7ecd3d3281c2957769ed430f2d7df0f99b874eea7d3965cbd03162040000acec354142400410d03dfa3dfa00408681fb11fb100304300f20802022a079907ac001410d03dfa3dfa00c08681fb11fb100704540f330f5d80402180790402410403c8201408201e410
00135442b3b0038529a0a7d30f4ca829f4409904540c593060829862cd58018159d86a7c0380082180790400811903ca2409200608601e4100404300f2080282180790401811903ca23d7200e08601e4100804300f2080
0014251dcd0c98d90b2226d313d2145b49a8a4547ffffffff0019bc4fdfa1fa1007fff8000000000
0013662222e3dfbb7ecd3d3281c2b57769ed430f2d7df0f9a7d30f4cb4fa72cb97a062d008000159d86a8b8480082180790400811503c8c3f6e00608601e4100404300f2080282180790401811503c8c3ef600e08601e4100804300f2080482080790402810403c820)";

const char* BATCH_JSON = R"({ "type": "SPAT", "timestamp": 1735713156407, "hex": "00136e2222e3dfbb7ecd3d3281c2357769ed430f2d7df0f970c1845c3cfa062d408010159d7bee304800821a06e806e8000810d03840399800608a81b5c1b5c00404340fab0fff802821a06e806e8001810d0383838d000e08681f561fff00804540dae0dae004821806c2402810c03612" }
{ "type": "SPAT", "timestamp": 1735713156408, "hex": "0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d7bee583000821a082d887f400810d03820384200608601b0a00604340e080e10804022a06f1c6f6002410c0361401408601b0a" }
{ "type": "SSM", "timestamp": 1735713156409, "hex": "001e1562b3af72a108000062d8081196a7200c0412001480" }
{ "type": "SPAT", "timestamp": 1735713156410, "hex": "0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d7c5bb83000821a082d887f400810d03820384200608601b2d00604340e080e10804022a06f1c6f6002410c0365a01408601b2d" }
{ "type": "BSM", "timestamp": 1735713156411, "hex": "0014250d4d0c9918eb5c26d0fa1f145efb53239ffffffffff00054a6fdfa1fa1007fff8000000000" })";

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode BSM", "[encoding]" ) { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();

//     std::stringstream out1;
//     CHECK(asn1_codec.file_test("unit-test-data/BSM.xml", out1) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out1, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

TEST_CASE("Encode ASD", "[encoding]" ) {
    std::cout << "=== Encode ASD ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out2;
    CHECK(asn1_codec.file_test("unit-test-data/ASD.xml", out2) == EXIT_SUCCESS);
    parse_result = output_doc.load(out2, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);
}

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode ASD_BSM", "[encoding]" ) { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();
//     std::stringstream out3;
//     CHECK(asn1_codec.file_test("unit-test-data/ASD_BSM.xml", out3) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out3, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("AdvisorySituationData").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

TEST_CASE("Encode 1609", "[encoding]" ) {
    std::cout << "=== Encode 1609 ===" << std::endl;
    
    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out4;
    CHECK(asn1_codec.file_test("unit-test-data/1609.xml", out4) == EXIT_SUCCESS);
    parse_result = output_doc.load(out4, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
}

TEST_CASE("Encode ASD_1609", "[encoding]" ) {
    std::cout << "=== Encode ASD_1609 ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

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
}

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode 1609_BSM", "[encoding]") { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();

//     std::stringstream out6;
//     CHECK(asn1_codec.file_test("unit-test-data/1609_BSM.xml", out6) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out6, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode ASD_1609_BSM", "[encoding]") { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();
    
//     std::stringstream out7;
//     CHECK(asn1_codec.file_test("unit-test-data/ASD_1609_BSM.xml", out7) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out7, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
//     byte_node = payload_node.child("AdvisorySituationData").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ASD_ONE609_HEX) == 0);
// }

TEST_CASE("Encode TIM with payload type 'OdeTimPayload'", "[encoding][odetimpayload]") {
    std::cout << "=== Encode TIM with payload type 'OdeTimPayload' ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out8;
    CHECK(asn1_codec.file_test("data/InputData.encoding.tim.odetimpayload.xml", out8) == EXIT_SUCCESS);
    parse_result = output_doc.load(out8, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

TEST_CASE("Encode TIM with payload type 'OdeAsdPayload'", "[encoding][odeasdpayload]") {
    std::cout << "=== Encode TIM with payload type 'OdeAsdPayload' ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out8;
    CHECK(asn1_codec.file_test("data/InputData.encoding.tim.odeasdpayload.xml", out8) == EXIT_SUCCESS);
    parse_result = output_doc.load(out8, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

// TODO check oracles with decoder

TEST_CASE("Decode BSM", "[decoding]") {
    std::cout << "=== Decode BSM ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

TEST_CASE("Decode BSM with VehicleEventFlags (hard braking event)", "[decoding]") {
    std::cout << "=== Decode BSM with VehicleEventFlags (hard braking event) ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.with.VehicleEventFlags.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    pugi::xml_node event_flags_node = bsm_vehicle_event_flags_query.evaluate_node(payload_node).node();
    CHECK(event_flags_node);
    std::string bitstring(event_flags_node.text().get());
    std::cout << "VehicleEventFlags: " << bitstring << std::endl;
    std::string expected_bitstring("0000000100000");
    CHECK(bitstring == expected_bitstring);
}

TEST_CASE("Decode BSM with VehicleEventFlags (jackknife event)", "[decoding]") {
    std::cout << "=== Decode BSM with VehicleEventFlags (jackknife event) ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.with.VehicleEventFlags.eventJackKnife.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    pugi::xml_node event_flags_node = bsm_vehicle_event_flags_query.evaluate_node(payload_node).node();
    CHECK(event_flags_node);
    std::string bitstring(event_flags_node.text().get());
    std::cout << "VehicleEventFlags: " << bitstring << std::endl;
    std::string expected_bitstring("00000000000001");
    CHECK(bitstring == expected_bitstring);
}

/*
 * Utilities for VehicleEventFlags tests
 */

void print_bits(const void * buf, const ssize_t num_bytes) {
    std::cout << "uper: ";
    for (int i = 0; i < num_bytes; i++) {
        uint8_t abyte = *((uint8_t *)buf + i);
        std::cout << std::bitset<8>(abyte) << " ";
    }
    std::cout << std::endl;
}

void test_encode_VehicleEventFlags_to_uper(const uint16_t bitstring, const uint8_t *expected_uper, const ssize_t expected_byte_len) {
    std::cout << "bitstring: " << std::bitset<16>(bitstring) << std::endl;
    const uint8_t buf[] = { static_cast<uint8_t>(bitstring >> 8), static_cast<uint8_t>(bitstring & 0xFF) };
    const size_t size = sizeof(buf);
    uint8_t *ptr_buf = (uint8_t *)&buf;
    const int bits_unused = __builtin_ctz(bitstring);
    asn_struct_ctx_t asn_ctx = {};
    VehicleEventFlags_t vef = {ptr_buf, size, bits_unused, asn_ctx};
    VehicleEventFlags_t* ptr_vef = &vef;
    asn_TYPE_descriptor_t *vef_type = &asn_DEF_VehicleEventFlags;
    asn_encode_to_new_buffer_result_t res
        = asn_encode_to_new_buffer(0, ATS_UNALIGNED_BASIC_PER, vef_type, ptr_vef);
    if (res.buffer) {
        printf("Successfully encoded %ld bytes to uper\n", res.result.encoded);
        print_bits(res.buffer, res.result.encoded);
    } else {
        FAIL("Error encoding vef");
        return;
    }
    if (res.result.encoded != expected_byte_len) {
        fprintf(stderr, "Error, expected %ld bytes but got %ld", expected_byte_len, res.result.encoded);
        FAIL("Wrong number of bytes");
    }
    if (memcmp(expected_uper, (uint8_t *)res.buffer, res.result.encoded) != 0) {
        FAIL("Error, Uper doesn't match expected");
    }
    free(res.buffer);
}

void test_decode_VehicleEventFlags_from_uper(const uint8_t * uper, const ssize_t uper_len, const uint16_t expected_bitstring,
    const int expect_constraint_to_fail) {
    print_bits(uper, uper_len);
    VehicleEventFlags_t * ptr_vef = 0;
    asn_dec_rval_t rval
        = asn_decode(0, ATS_UNALIGNED_BASIC_PER, &asn_DEF_VehicleEventFlags, (void **)&ptr_vef, uper, uper_len);
    if (rval.code != RC_OK) {
        FAIL("Error decoding");
        return;
    }
    printf("Successfully decoded %ld bytes\n", rval.consumed);
    printf("bitstring size: %ld, bits_unused: %d\n", ptr_vef->size, ptr_vef->bits_unused);

    if (ptr_vef->size != 2) {
        FAIL("Error, expected bitstring to be 2 bytes");
    }
    uint16_t actual_bitstring = (ptr_vef->buf[0] << 8) | ptr_vef->buf[1];
    std::cout << "bitstring: " << std::bitset<16>(actual_bitstring) << std::endl;
    if (actual_bitstring != expected_bitstring) {
        std::cout << "Error, expected bitstring: " << std::bitset<16>(expected_bitstring)
            << " but got: " << std::bitset<16>(actual_bitstring) << std::endl;
        FAIL("Bitstring mismatch");
    }

    // Check constraints
    char errbuf[128];
    size_t errlen = sizeof(errbuf);
    int cons_ret = asn_check_constraints(&asn_DEF_VehicleEventFlags, ptr_vef, errbuf, &errlen);
    if (cons_ret && !expect_constraint_to_fail) {
        fprintf(stderr, "Constraint check failed: %s\n", errbuf);
        FAIL("Unexpected constraint check fail");
    }

    ASN_STRUCT_FREE(asn_DEF_VehicleEventFlags, ptr_vef);
}

/*
 * Data for VehicleEventFlags tests
 */
constexpr uint16_t bitstring_root = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                        | (0x8000 >> VehicleEventFlags_eventAirBagDeployment);
constexpr uint8_t uper_root[2] = { 0b01000000, 0b00000100 };

constexpr uint16_t bitstring_with_ext = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                        | (0x8000 >> VehicleEventFlags_eventAirBagDeployment)
                                        | (0x8000 >> VehicleEventFlags_eventJackKnife);

constexpr uint8_t uper_with_ext[3] = { 0b10000111, 0b01000000, 0b00000110 };

constexpr uint16_t bitstring_15bits = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                      | (0x8000 >> VehicleEventFlags_eventAirBagDeployment)
                                      | (0x8000 >> VehicleEventFlags_eventJackKnife)
                                      | (0x8000 >> 14);
constexpr uint8_t uper_15bits[3] = {0b10000111, 0b11000000, 0b00000111 };

TEST_CASE("Encode VehicleEventFlags root, from struct to UPER, 13 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags root, from struct to UPER, 13 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_root, uper_root, 2);
}

TEST_CASE("Encode VehicleEventFlags with 2024 extension, from struct to UPER, 14 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags with 2024 extension to UPER, 14 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_with_ext, uper_with_ext, 3);
}

TEST_CASE("Encode VehicleEventFlags with unknown future extension, from struct to UPER, 15 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags with unknown future extension, from struct to UPER, 15 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_15bits, uper_15bits, 3);
}

TEST_CASE("Decode VehicleEventFlags root, from UPER to struct, 13 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags root, from UPER to struct, 13 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_root, 2, bitstring_root, 0);
}

TEST_CASE("Decode VehicleEventFlags with 2024 extension, from UPER to struct, 14 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags with 2024 extension, from UPER to struct, 14 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_with_ext, 3, bitstring_with_ext, 0);
}

TEST_CASE("Decode VehicleEventFlags with unknown future extension, from UPER to struct, 15 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags with unknown future extension, from UPER to struct, 15 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_15bits, 3, bitstring_15bits, 1);
}


TEST_CASE("Http_Server::post_single: Decode BSM", "[decoding][http_server]") {
    std::cout << "=== HttpServer::post_single: Decode SPAT ===" << std::endl;

    Http_Server server(asn1_codec);

    // Set up request
    crow::request req;
    req.add_header("Content-Type", "text/plain");
    req.body = std::string(BSM_HEX);

    crow::response response = server.post_single(req);

    // Is OK response containing decoded BSM?
    CHECK(response.code == 200);
    CHECK(response.body.find("<BasicSafetyMessage>") != std::string::npos);
}

TEST_CASE("Http_Server::post_batch (text/plain)", "[decoding][http_server]") {
    std::cout << "=== HttpServer::post_batch (text/plain)" << std::endl;

    Http_Server server(asn1_codec);

    crow::request req;
    req.add_header("Content-Type", "text/plain");
    req.body = std::string(BATCH_HEX);

    crow::response response = server.post_batch(req);

    CHECK(response.code == 200);
    CHECK(response.body.find("<BasicSafetyMessage>") != std::string::npos);
    CHECK(response.body.find("<SPAT>") != std::string::npos);
}

TEST_CASE("Http_Server::post_batch (application/x-ndjson)", "[decoding][http_server]") {
    std::cout << "=== HttpServer::post_batch (application/x-ndjson)" << std::endl;

    Http_Server server(asn1_codec);

    crow::request req;
    req.add_header("Content-Type", "application/x-ndjson");
    req.body = std::string(BATCH_JSON);

    crow::response response = server.post_batch(req);

    CHECK(response.code == 200);
    CHECK(response.body.find("<BasicSafetyMessage>") != std::string::npos);
    CHECK(response.body.find("<SPAT>") != std::string::npos);
    CHECK(response.body.find("<SignalStatusMessage>") != std::string::npos);
}