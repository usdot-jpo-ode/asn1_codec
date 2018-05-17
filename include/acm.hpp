/** 
 * @file 
 * @author   Jason M. Carter (carterjm@ornl.gov)
 * @author   Aaron E. Ferber (ferberae@ornl.gov)
 * @date     Sept 11, 2017
 * @version  0.1
 *
 * @copyright Copyright 2017 US DOT - Joint Program Office
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Contributors:
 *    Oak Ridge National Laboratory, Center for Trustworthy Embedded Systems, UT Battelle.
 *
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2014, Magnus Edenhill
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * TODO: All the xpath expressions should be input parameters.
 * TODO: All the fixed XML element names should be input parameters, e.g., Ieee1609Dot2Data, encodingRule.
 * TODO: Test the constraints checker; not sure how to build test data.
 * TODO: Add to docs, if your encoding rules are WRONG, you will get a bad data error -- the data may be good under another set of encoding rules.
 */

#include "MessageFrame.h"
#include "Ieee1609Dot2Data.h"
#include "AdvisorySituationData.h"
#include "tool.hpp"
#include "spdlog/spdlog.h"
#include "rdkafkacpp.h"
#include "pugixml.hpp"

#include <deque>
#include <utility>
#include <tuple>
#include <sstream>

typedef struct buffer_structure {
    char *buffer;
    size_t buffer_size;      // this is really where we will write next.
    size_t allocated_size;   // this is the total size of the buffer.
} buffer_structure_t;

enum class Asn1ErrorType : uint32_t {
    SUCCESS = 0,            // not used.
    FAILURE,                // not used.
    REQUEST,                // Any error relating to the input XML.
    DATA,                   // Any error relating to the payload in the input XML.
    COUNT
};

enum class Asn1DataType : uint32_t {
    ODE = 0,                // return data type for all error messages.
    XML,                    // return data type for XML/MessageFrame data.
    HEX,                    // hex byte array data type.
    PAYLOAD,                // payload type.
    COUNT
};

// an enumeration that specifies which bit in a flag word is used to turn on and off certain operations.
enum class Asn1OpsType : uint32_t {
	IEEE1609DOT2 = 1,			// 1<<0
	J2735MESSAGEFRAME = 2,		// 1<<1
	ASDFRAME = 4,					// 1<<2
	COUNT
};

class UnparseableInputError : public std::runtime_error {

    Asn1DataType dt_;
    Asn1ErrorType et_;

    public:

        explicit UnparseableInputError( const char* message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::REQUEST ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        explicit UnparseableInputError( const std::string& message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::REQUEST  ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        virtual ~UnparseableInputError() throw () 
        { }

        Asn1DataType data_type() const 
        {
            return dt_;
        }

        Asn1ErrorType error_type() const 
        {
            return et_;
        }
};

class MissingInputElementError : public std::runtime_error {

    Asn1DataType dt_;
    Asn1ErrorType et_;

    public:

        explicit MissingInputElementError( const char* message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::REQUEST ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        explicit MissingInputElementError( const std::string& message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::REQUEST  ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        virtual ~MissingInputElementError() throw () 
        { }

        Asn1DataType data_type() const 
        {
            return dt_;
        }

        Asn1ErrorType error_type() const 
        {
            return et_;
        }
};

class Asn1CodecError : public std::runtime_error {
    Asn1DataType dt_;
    Asn1ErrorType et_;

    public:

        explicit Asn1CodecError( const char* message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::DATA ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        explicit Asn1CodecError( const std::string& message, Asn1DataType dt = Asn1DataType::ODE, Asn1ErrorType et = Asn1ErrorType::DATA  ) :
            std::runtime_error{ message }
            , dt_{ dt }
            , et_{ et }
        {}

        virtual ~Asn1CodecError() throw () 
        { }

        Asn1DataType data_type() const 
        {
            return dt_;
        }

        Asn1ErrorType error_type() const 
        {
            return et_;
        }
};

class ASN1_Codec : public tool::Tool {

    public:

        std::shared_ptr<spdlog::logger> ilogger;
        std::shared_ptr<spdlog::logger> elogger;

        static void sigterm (int sig);

        ASN1_Codec( const std::string& name, const std::string& description );
        ~ASN1_Codec();
        void metadata_print (const std::string &topic, const RdKafka::Metadata *metadata);
        bool topic_available( const std::string& topic );
        void print_configuration() const;
        bool configure();
        bool launch_consumer();
        bool launch_producer();
        bool process_message(RdKafka::Message* message, std::stringstream& output_message_stream);
        bool filetest();
        bool file_test(std::string file_path, std::ostream& os, bool encode = true);
        int operator()(void);

        /**
         * @brief Create and setup the two loggers used for the ASN1_Codec. The locations and filenames for the logs can be specified
         * using command line parameters. The CANNOT be set via the configuration file, since these loggers are setup
         * prior to the configuration file being read.
         *
         * If the log directory does not exist it will be created.
         *
         * Log files will be appended to, unless specified by a command line option.
         *
         * @return true upon success; false if some failure occurred during logger setup.
         */
        bool make_loggers( bool remove_files );

    private:

        static bool bootstrap;                                          ///> flag indicating we need to bootstrap the consumer and producer
        static bool data_available;                                     ///> flag to exit application; set via signals so static.

        static constexpr long ilogsize = 1048576 * 5;                   ///> The size of a single information log; these rotate.
        static constexpr long elogsize = 1048576 * 2;                   ///> The size of a single error log; these rotate.
        static constexpr int ilognum = 5;                               ///> The number of information logs to rotate.
        static constexpr int elognum = 2;                               ///> The number of error logs to rotate.
        static constexpr std::size_t max_errbuf_size = 128;             ///> The length of error buffers for ASN.1 compiler.

        // possible encoding configurations.
        static constexpr uint32_t IEEE1609DOT2 = 1;
        static constexpr uint32_t J2735MESSAGEFRAME = 2;		
        static constexpr uint32_t IEEE1609DOT2_J2735MESSAGEFRAME = 3;
        static constexpr uint32_t ASDFRAME = 4;
        static constexpr uint32_t ASDFRAME_IEEE1609DOT2 = 5;
        static constexpr uint32_t ASDFRAME_J2735MESSAGEFRAME = 6;
        static constexpr uint32_t ASDFRAME_IEEE1609DOT2_J2735MESSAGEFRAME = 7;

        bool exit_eof;                                                  ///> flag to cause the application to exit on stream eof.
        int32_t eof_cnt;                                                ///> counts the number of eofs needed for exit_eof to work; each partition must end.
        int32_t partition_cnt;                                          ///> TODO: the number of partitions being processed; currently 1.

        // bookkeeping.
        uint64_t msg_recv_count;                                        ///> Counter for the number of BSMs received.
        uint64_t msg_send_count;                                        ///> Counter for the number of BSMs published.
        uint64_t msg_filt_count;                                        ///> Counter for hte number of BSMs filtered/suppressed.
        uint64_t msg_recv_bytes;                                        ///> Counter for the number of BSM bytes received.
        uint64_t msg_send_bytes;                                        ///> Counter for the nubmer of BSM bytes published.
        uint64_t msg_filt_bytes;                                        ///> Counter for the nubmer of BSM bytes filtered/suppressed.

        // Logging.
        spdlog::level::level_enum iloglevel;                            ///> Log level for the information log.
        spdlog::level::level_enum eloglevel;                            ///> Log level for the error log.
        std::string mode;
        std::string debug;

        // configurations; global and topic (the names in these are fixed)
        std::unordered_map<std::string, std::string> pconf;
        RdKafka::Conf *conf;
        RdKafka::Conf *tconf;

        // Kafka component pointers and variables.
        int consumer_timeout;
        std::string brokers;
        int32_t partition;
        int64_t offset;
        std::string published_topic_name;                               ///> The topic we are publishing filtered BSM to.
        std::vector<std::string> consumed_topics;                       ///> consumer topics.
        std::shared_ptr<RdKafka::KafkaConsumer> consumer_ptr;
        std::shared_ptr<RdKafka::Producer> producer_ptr;
        std::shared_ptr<RdKafka::Topic> published_topic_ptr;

        // ODE XML input XPath queries and parse options.
        pugi::xml_document input_doc;
        pugi::xml_document internal_doc;
        pugi::xml_document error_doc;                                   ///> A base XML document to use in responding to input XML parse errors.

        unsigned int xml_parse_options;
        pugi::xpath_query ieee1609dot2_unsecuredData_query;
        pugi::xpath_query ode_payload_query;
        pugi::xpath_query ode_encodings_query;

        std::ostringstream erroross;
		bool add_error_xml( pugi::xml_document& doc, Asn1DataType dt, Asn1ErrorType et, std::string message, bool update_time = false );

        std::vector<char> byte_buffer;                                 ///> storage for hex to byte and byte to hex encoder/decoder.


        bool hex_to_bytes_(const std::string& payload_hex, std::vector<char>& byte_buffer);
        bool bytes_to_hex_(buffer_structure_t* buf_struct, std::string& payload_hex );

        // ASN.1 Compiler
        std::size_t errlen;
        char errbuf[max_errbuf_size];

		// TODO: A byte flag word is needed here since we will set multiple decode / encoders.
		uint32_t opsflag;
        bool decode_1609dot2;
        bool decode_messageframe;
		bool decode_asdframe;
        bool decode_functionality;

		// TODO: flagword is more challenging here...
        enum asn_transfer_syntax decode_1609dot2_type;
        enum asn_transfer_syntax decode_messageframe_type;
        enum asn_transfer_syntax decode_asdframe_type;
        enum asn_transfer_syntax curr_decode_type_;
    
        uint32_t curr_op_;
        std::string curr_node_path_;
        pugi::xml_node payload_node_;

        std::vector<std::tuple<uint32_t, enum asn_transfer_syntax, std::string, bool>> protocol_;
        std::vector<std::tuple<std::string, std::string>> hex_data_;

        enum asn_transfer_syntax get_ats_transfer_syntax( const char* ats_type );
        bool set_codec_requirements( pugi::xml_document& doc );

        bool decode_message( pugi::xml_node& payload_node, std::stringstream& output_message_stream );
        bool decode_message_legacy( pugi::xml_node& payload_node, std::stringstream& output_message_stream );
        bool decode_1609dot2_data( std::string& data_as_hex, buffer_structure_t* xml_buffer );
        bool decode_messageframe_data( std::string& data_as_hex, buffer_structure_t* xml_buffer );

        bool encode_message( std::stringstream& output_message_stream );
        void encode_frame_data(const std::string& data_as_xml, std::string& hex_string);
        void encode_node_as_hex_string(bool replace = true);
        void encode_for_protocol();

        std::string get_current_time() const;
};

