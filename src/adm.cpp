/** 
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

#include "adm.hpp"
#include "utilities.hpp"
#include <iomanip>

#include "spdlog/spdlog.h"

#include <csignal>
#include <chrono>
#include <thread>
#include <cstdio>

// for both windows and linux.
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#include <atltime.h>
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#include <unistd.h>
#endif

/**
 * @brief predicate indicating whether a file exists on the filesystem.
 *
 * @return true if it exists, false otherwise.
 */
bool fileExists( const std::string& s ) {
    struct stat info;

    if (stat( s.c_str(), &info) != 0) {     // bad stat; doesn't exist.
        return false;
    } else if (info.st_mode & S_IFREG) {    // exists and is a regular file.
        return true;
    } 

    return false;
}

/**
 * @brief predicate indicating whether a path/directory exists on the filesystem.
 *
 * @return true if it exists, false otherwise.
 */
bool dirExists( const std::string& s ) {
    struct stat info;

    if (stat( s.c_str(), &info) != 0) {     // bad stat; doesn't exist.
        return false;
    } else if (info.st_mode & S_IFDIR) {    // exists and is a directory.
        return true;
    } 

    return false;
}

/**
 *
 * TODO: Consider moving these to the ASN1_Codec class?
 */
static int xer_buffer_append(const void *buffer, size_t size, void *app_key) {
    xer_buffer_t *xb = static_cast<xer_buffer_t *>(app_key);

    while(xb->buffer_size + size + 1 > xb->allocated_size) {
        // increase size of buffer.
        size_t new_size = 2 * (xb->allocated_size ? xb->allocated_size : 64);
        char *new_buf = static_cast<char *>(MALLOC(new_size));
        if(!new_buf) return -1;
        // move old to new.
        memcpy(new_buf, xb->buffer, xb->buffer_size);

        FREEMEM(xb->buffer);
        xb->buffer = new_buf;
        xb->allocated_size = new_size;
    }

    memcpy(xb->buffer + xb->buffer_size, buffer, size);
    xb->buffer_size += size;
    // null terminate the string.
    xb->buffer[xb->buffer_size] = '\0';
    return 0;
}

bool ASN1_Codec::data_available = true;

void ASN1_Codec::sigterm (int sig) {
    data_available = false;
}

ASN1_Codec::ASN1_Codec( const std::string& name, const std::string& description ) :
    Tool{ name, description },
    exit_eof{true},
    eof_cnt{0},
    partition_cnt{1},
    msg_recv_count{0},
    msg_send_count{0},
    msg_filt_count{0},
    msg_recv_bytes{0},
    msg_send_bytes{0},
    msg_filt_bytes{0},
    iloglevel{ spdlog::level::trace },
    eloglevel{ spdlog::level::err },
    pconf{},
    brokers{"localhost"},
    partition{RdKafka::Topic::PARTITION_UA},
    mode{""},
    debug{""},
    consumed_topics{},
    offset{RdKafka::Topic::OFFSET_BEGINNING},
    published_topic_name{},
    conf{nullptr},
    tconf{nullptr},
    consumer_ptr{},
    consumer_timeout{500},
    producer_ptr{},
    published_topic_ptr{},
    ieee1609dot2_unsecuredData_query{"Ieee1609Dot2Data/content//unsecuredData"},  // this will work on both signed and unsigned.
    ode_payload_query{"OdeAsn1Data/payload/data"},
    ode_encodings_query{"OdeAsn1Data/metadata/encodings"},
    element_type_stack{},
    doc_stack{},
    xml_parse_options{ pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata },
    ilogger{},
    elogger{}
{
    // dump_file = fopen( "dump.file.dat", "wb" );
}

ASN1_Codec::~ASN1_Codec() 
{
    // fclose(dump_file);

    if (consumer_ptr) {
        consumer_ptr->close();
    }

    // free raw librdkafka pointers.
    if (tconf) delete tconf;
    if (conf) delete conf;

    // TODO: This librdkafka item seems wrong...
    RdKafka::wait_destroyed(5000);    // pause to let RdKafka reclaim resources.
}

void ASN1_Codec::metadata_print (const std::string &topic, const RdKafka::Metadata *metadata) {

    std::cout << "Metadata for " << (topic.empty() ? "" : "all topics")
        << "(from broker "  << metadata->orig_broker_id()
        << ":" << metadata->orig_broker_name() << std::endl;

    /* Iterate brokers */
    std::cout << " " << metadata->brokers()->size() << " brokers:" << std::endl;

    for ( auto ib : *(metadata->brokers()) ) {
        std::cout << "  broker " << ib->id() << " at " << ib->host() << ":" << ib->port() << std::endl;
    }

    /* Iterate topics */
    std::cout << metadata->topics()->size() << " topics:" << std::endl;

    for ( auto& it : *(metadata->topics()) ) {

        std::cout << "  topic \""<< it->topic() << "\" with " << it->partitions()->size() << " partitions:";

        if (it->err() != RdKafka::ERR_NO_ERROR) {
            std::cout << " " << err2str(it->err());
            if (it->err() == RdKafka::ERR_LEADER_NOT_AVAILABLE) std::cout << " (try again)";
        }

        std::cout << std::endl;

        /* Iterate topic's partitions */
        for ( auto& ip : *(it->partitions()) ) {
            std::cout << "    partition " << ip->id() << ", leader " << ip->leader() << ", replicas: ";

            /* Iterate partition's replicas */
            RdKafka::PartitionMetadata::ReplicasIterator ir;
            for (ir = ip->replicas()->begin(); ir != ip->replicas()->end(); ++ir) {
                std::cout << (ir == ip->replicas()->begin() ? "":",") << *ir;
            }

            /* Iterate partition's ISRs */
            std::cout << ", isrs: ";
            RdKafka::PartitionMetadata::ISRSIterator iis;
            for (iis = ip->isrs()->begin(); iis != ip->isrs()->end() ; ++iis)
                std::cout << (iis == ip->isrs()->begin() ? "":",") << *iis;

            if (ip->err() != RdKafka::ERR_NO_ERROR)
                std::cout << ", " << RdKafka::err2str(ip->err()) << std::endl;
            else
                std::cout << std::endl;
        }
    }
}

bool ASN1_Codec::topic_available( const std::string& topic ) {
    bool r = false;

    RdKafka::Metadata* md;
    RdKafka::ErrorCode err = consumer_ptr->metadata( true, nullptr, &md, 5000 );
    // TODO: Will throw a broker transport error (ERR__TRANSPORT = -195) if the broker is not available.

    if ( err == RdKafka::ERR_NO_ERROR ) {
        RdKafka::Metadata::TopicMetadataIterator it = md->topics()->begin();

        // search for the raw BSM topic.
        while ( it != md->topics()->end() && !r ) {
            // finish when we find it.
            r = ( (*it)->topic() == topic );
            if ( r ) ilogger->info( "Topic: {} found in the kafka metadata.", topic );
            ++it;
        }
        if (!r) ilogger->warn( "Metadata did not contain topic: {}.", topic );

    } else {
        elogger->error( "cannot retrieve consumer metadata with error: {}.", err2str(err) );
    }
    
    return r;
}

void ASN1_Codec::print_configuration() const
{
    std::cout << "# Global config" << "\n";
    std::list<std::string>* conf_list = conf->dump();

    int i = 0;
    for ( auto& v : *conf_list ) {
        if ( i%2==0 ) std::cout << v << " = ";
        else std::cout << v << '\n';
        ++i;
    }

    std::cout << "# Topic config" << "\n";
    conf_list = tconf->dump();
    i = 0;
    for ( auto& v : *conf_list ) {
        if ( i%2==0 ) std::cout << v << " = ";
        else std::cout << v << '\n';
        ++i;
    }

    std::cout << "# Privacy config \n";
    for ( const auto& m : pconf ) {
        std::cout << m.first << " = " << m.second << '\n';
    }
}


/**
 * JMC: asn1 reviewed.
 *
 * The following configuration settings are processed by configure:
 *
 asn1.j2735.kafka.partition
 asn1.j2735.topic.consumer
 asn1.j2735.topic.producer
 asn1.j2735.consumer.timeout.ms

JMC: TODO: All the ASN1C variables that are normally setup do here!

*/
bool ASN1_Codec::configure() {

    if ( optIsSet('v') ) {
        if ( "trace" == optString('v') ) {
            ilogger->set_level( spdlog::level::trace );
        } else if ( "debug" == optString('v') ) {
            ilogger->set_level( spdlog::level::trace );
        } else if ( "info" == optString('v') ) {
            ilogger->set_level( spdlog::level::trace );
        } else if ( "warning" == optString('v') ) {
            ilogger->set_level( spdlog::level::warn );
        } else if ( "error" == optString('v') ) {
            ilogger->set_level( spdlog::level::err );
        } else if ( "critical" == optString('v') ) {
            ilogger->set_level( spdlog::level::critical );
        } else if ( "off" == optString('v') ) {
            ilogger->set_level( spdlog::level::off );
        } else {
            elogger->warn("information logger level was configured but unreadable; using default.");
        }
    } // else it is already set to default.

    ilogger->trace("starting configure()");

    std::string line;
    std::string error_string;
    StrVector pieces;

    // configurations; global and topic (the names in these are fixed)
    conf  = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // must use a configuration file.
    if ( !optIsSet('c') ) {
        elogger->error( "asked to use a configuration file, but option not set." );
        return false;
    }

    const std::string& cfile = optString('c');              // needed for error message.
    ilogger->info("using configuration file: {}", cfile );
    std::ifstream ifs{ cfile };

    if (!ifs) {
        elogger->error("cannot open configuration file: {}", cfile);
        return false;
    }

    while (std::getline( ifs, line )) {
        line = string_utilities::strip( line );
        if ( !line.empty() && line[0] != '#' ) {
            pieces = string_utilities::split( line, '=' );
            bool done = false;
            if (pieces.size() == 2) {
                // in case the user inserted some spaces...
                string_utilities::strip( pieces[0] );
                string_utilities::strip( pieces[1] );
                // some of these configurations are stored in each...?? strange.
                if ( tconf->set(pieces[0], pieces[1], error_string) == RdKafka::Conf::CONF_OK ) {
                    ilogger->info("kafka topic configuration: {} = {}", pieces[0], pieces[1]);
                    done = true;
                }

                if ( conf->set(pieces[0], pieces[1], error_string) == RdKafka::Conf::CONF_OK ) {
                    ilogger->info("kafka configuration: {} = {}", pieces[0], pieces[1]);
                    done = true;
                }

                if ( !done ) { 
                    ilogger->info("ASN1_Codec configuration: {} = {}", pieces[0], pieces[1]);
                    // These configuration options are not expected by Kafka.
                    // Assume there are for the ASN1_Codec.
                    pconf[ pieces[0] ] = pieces[1];
                }

            } else {
                elogger->warn("too many pieces in the configuration file line: {}", line);
            }

        } // otherwise: empty or comment line.
    }

    // All configuration file settings are overridden, if supplied, by CLI options.

    if ( optIsSet('b') ) {
        // broker specified.
        ilogger->info("setting kafka broker to: {}", optString('b'));
        conf->set("metadata.broker.list", optString('b'), error_string);
    } 

    if ( optIsSet('p') ) {
        // number of partitions.
        partition = optInt( 'p' );

    } else {
        auto search = pconf.find("asn1.j2735.kafka.partition");
        if ( search != pconf.end() ) {
            partition = std::stoi(search->second);              // throws.    
        }  // otherwise leave at default; PARTITION_UA
    }

    ilogger->info("kafka partition: {}", partition);

    if ( getOption('g').isSet() && conf->set("group.id", optString('g'), error_string) != RdKafka::Conf::CONF_OK) {
        // NOTE: there are some checks in librdkafka that require this to be present and set.
        elogger->error("kafka error setting configuration parameters group.id h: {}", error_string);
        return false;
    }

    if ( getOption('o').isSet() ) {
        // offset in the consumed stream.
        std::string o = optString( 'o' );

        if (o=="end") {
            offset = RdKafka::Topic::OFFSET_END;
        } else if (o=="beginning") {
            offset = RdKafka::Topic::OFFSET_BEGINNING;
        } else if (o== "stored") {
            offset = RdKafka::Topic::OFFSET_STORED;
        } else {
            offset = strtoll(o.c_str(), NULL, 10);              // throws
        }

        ilogger->info("offset in partition set to byte: {}", o);
    }

    // Do we want to exit if a stream eof is sent.
    exit_eof = getOption('x').isSet();

    if (optIsSet('d') && conf->set("debug", optString('d'), error_string) != RdKafka::Conf::CONF_OK) {
        elogger->error("kafka error setting configuration parameter debug: {}", error_string);
        return false;
    }

    // librdkafka defined configuration.
    conf->set("default_topic_conf", tconf, error_string);

    auto search = pconf.find("asn1.j2735.topic.consumer");
    if ( search != pconf.end() ) {
        consumed_topics.push_back( search->second );
        ilogger->info("consumed topic: {}", search->second);

    } else {
        
        elogger->error("no consumer topic was specified; must fail.");
        return false;
    }

    if (optIsSet('t')) {
        // this is the produced (filtered) topic.
        published_topic_name = optString( 't' );

    } else {
        // maybe it was specified in the configuration file.
        auto search = pconf.find("asn1.j2735.topic.producer");
        if ( search != pconf.end() ) {
            published_topic_name = search->second;
        } else {
            elogger->error("no publisher topic was specified; must fail.");
            return false;
        }
    }

    ilogger->info("published topic: {}", published_topic_name);

    search = pconf.find("asn1.j2735.consumer.timeout.ms");
    if ( search != pconf.end() ) {
        try {
            consumer_timeout = std::stoi( search->second );
        } catch( std::exception& e ) {
            ilogger->info("using the default consumer timeout value.");
        }
    }

    ilogger->trace("ending configure()");
    return true;
}

enum asn_transfer_syntax ASN1_Codec::get_ats_transfer_syntax( const char* ats ) {

    enum asn_transfer_syntax r = ATS_INVALID;

    if ( std::strcmp( ats, "UPER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: UPER");
        r = ATS_UNALIGNED_BASIC_PER;

    } if ( std::strcmp( ats, "COER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: COER");
        r = ATS_CANONICAL_OER;

    } if ( std::strcmp( ats, "XER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: XER");
        r = ATS_BASIC_XER;

    } if ( std::strcmp( ats, "CPER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: CPER");
        r = ATS_UNALIGNED_CANONICAL_PER;

    } if ( std::strcmp( ats, "CXER" ) == 0 ) {
        
        ilogger->trace("resolved ATS Code: CXER");
        r = ATS_CANONICAL_XER;

    } if ( std::strcmp( ats, "BER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: BER");
        r = ATS_BER;

    } if ( std::strcmp( ats, "DER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: DER");
        r = ATS_DER;

    } if ( std::strcmp( ats, "CER" ) == 0 ) {

        ilogger->trace("resolved ATS Code: CER");
        r = ATS_CER;

    } else {
        ilogger->trace("resolved ATS Code: INVALID");
    }

    ilogger->trace("resolved ATS Code: {}", r);
    return r;
}

bool ASN1_Codec::msg_consume(RdKafka::Message* message, std::stringstream& xmlss ) {

    static std::string tsname;
    static RdKafka::MessageTimestamp ts;
    
    // flags for type of decoding required.
    bool decode_1609dot2 = false;
    bool decode_messageframe = false;
    enum asn_transfer_syntax decode_1609dot2_type = ATS_CANONICAL_OER;
    enum asn_transfer_syntax decode_messageframe_type = ATS_UNALIGNED_BASIC_PER;

    size_t bytes_processed = 0;

    switch (message->err()) {

        case RdKafka::ERR__TIMED_OUT:
            ilogger->info("Waiting for more BSMs from the ODE producer.");
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            msg_recv_count++;
            msg_recv_bytes += message->len();

            ilogger->trace("Read message at byte offset: {} with length {}", message->offset(), message->len() );

            ts = message->timestamp();

            if (ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
                if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME) {
                    tsname = "create time";
                } else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME) {
                    tsname = "log append time";
                } else {
                    tsname = "unknown";
                }

                ilogger->trace("Message timestamp: {}, type: {}", tsname, ts.timestamp);
            }

            if ( message->key() ) {
                ilogger->trace("Message key: {}", *message->key() );
            }

            if ( message->len() > 0 ) {

                xer_buffer_t xb = {0, 0, 0};

                // Load BAH XML Input Document.
                pugi::xml_document input_doc{};
                pugi::xml_parse_result result = input_doc.load_buffer((const void*) message->payload(), message->len(), xml_parse_options );

                if (!result) {
                    ilogger->trace("Error parsing consumed XML file: {} (offset = {})!", result.description(), result.offset);
                    return false; 
                } 

                // Determine which decodings are needed.
                pugi::xpath_node encodings_xpath_node = ode_encodings_query.evaluate_node( input_doc );

                enum asn_transfer_syntax atstype = ATS_INVALID;

                for ( pugi::xml_node n = encodings_xpath_node.node().first_child(); n; n = n.next_sibling()) {
    
                    pugi::xml_text ats_node = n.child("encodingRule").text();
                    if ( ats_node ) {
                        atstype = get_ats_transfer_syntax( ats_node.get() );
                    }

                    ilogger->trace("Inside the loop.");
                    std::cout << n.child("elementType").text().get() << '\n';
                    if ( std::strcmp(n.child("elementType").text().get(), "Ieee1609Dot2Data") == 0 ) {
                        decode_1609dot2 = true;
                        if ( atstype != ATS_INVALID ) {
                            // only change the default values if a new type is provided in the XML.
                            decode_1609dot2_type = atstype;
                        }
                        ilogger->trace("Setting the decode 1609.2 flag.");
                    } else if ( std::strcmp(n.child("elementType").text().get(), "MessageFrame") == 0 ) {
                        decode_messageframe = true;
                        if ( atstype != ATS_INVALID ) {
                            // only change the default values if a new type is provided in the XML.
                            decode_messageframe_type = atstype;
                        }
                        ilogger->trace("Setting the decode message frame flag.");
                    }
                }

                pugi::xpath_node payload_xpath_node = ode_payload_query.evaluate_node( input_doc );

                // This node is where the resultant data is written; save for future reference.
                pugi::xml_node payload_node = payload_xpath_node.node();

                // access this directly because we remove the bytes branch.
                pugi::xml_text text = payload_node.child("bytes").text();

                if ( !text ) {
                    elogger->warn("No text node (data source) in the consumed XML file.");
                    return false;
                }

                std::string hstr{ text.get() };

                // declare outside of the below scopes and reuse.
                pugi::xml_document internal_doc{};

                // first must be 1609dot2
                if ( decode_1609dot2 ) {

                    if ( decode_1609dot2_data(hstr, &xb, decode_1609dot2_type ) == false ) {
                        elogger->warn("Failure to extract/convert consumed XML file payload into a byte buffer.");
                        return false;
                    }

                    //pugi::xml_document ieee_doc{};
                    result = internal_doc.load_buffer(static_cast<const void *>( xb.buffer), xb.buffer_size );

                    std::free( static_cast<void *>(xb.buffer) );

                    if (!result) {
                        ilogger->trace("Error parsing IEEE 1609.2 XML data: {} (offset = {})!", result.description(), result.offset);
                        return false; 
                    } 

                    // reset the buffer.
                    xb = { 0,0,0 };

                    // XPath search the IEEE structure for the unsecured data.
                    pugi::xpath_node unsecuredDataNode = ieee1609dot2_unsecuredData_query.evaluate_node( internal_doc );
                    text = unsecuredDataNode.node().text();

                    if ( !text ) {
                        elogger->warn("No text node (data source) found in the IEEE 1609.2 XML document.");
                        return false;
                    }

                    hstr = std::string( text.get() );
                    // done with the 1609.2 DOM.
                }

                internal_doc.reset();

                if ( decode_messageframe ) {
                    if ( decode_messageframe_data( hstr, &xb, decode_messageframe_type ) == false ) {
                        elogger->warn("Failure to extract/convert IEEE 1609.2 XML file payload into a byte buffer.");
                        return false;
                    }

                    ilogger->info( "IEEE 1609.2 Unsecured Data Field Payload decode/encode operation complete (usually a J2735 MessageFrame)." );

                    // zero out hex string.
                    payload_node.text().set("");

                    // build an XML document from the ascii data.
                    // pugi::xml_document messageframexml{};
                    result = internal_doc.load_buffer( static_cast<const void *>( xb.buffer), xb.buffer_size );
                    std::free( static_cast<void *>(xb.buffer) );

                    if (!result) {
                        ilogger->trace("Error parsing J2735 XML data: {} (offset = {})!", result.description(), result.offset);
                        return false; 
                    } 

                }

                // replace the hex data with XML data.
                payload_node.remove_child("bytes");

                if ( internal_doc ) {
                    // TODO: make sure this is checking for empty document cases.
                    payload_node.append_copy( internal_doc.document_element() );
                } else {  // else doc the remaining document should be empty now!
                    ilogger->warn("Internal doc is empty.");
                    elogger->info("Internal doc is empty.");
                }

                // convert DOM to a RAW string representation: no spaces, no tabs.
                xmlss.clear();
                input_doc.save(xmlss,"",pugi::format_raw);
                return true;
            } 

            break;

        case RdKafka::ERR__PARTITION_EOF:
            ilogger->info("ODE BSM consumer partition end of file, but ASN1_Codec still alive.");
            if (exit_eof) {
                eof_cnt++;

                if (eof_cnt == partition_cnt) {
                    ilogger->info("EOF reached for all {} partition(s)", partition_cnt);
                    data_available = false;
                }
            }
            break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
            elogger->error("cannot consume due to an UNKNOWN consumer topic: {}", message->errstr());

        case RdKafka::ERR__UNKNOWN_PARTITION:
            elogger->error("cannot consume due to an UNKNOWN consumer partition: {}", message->errstr());
            data_available = false;
            break;

        default:
            elogger->error("cannot consume due to an error: {}", message->errstr());
            data_available = false;
    }

    return false;
}

/**
 * JMC: asn1 reviewed.
 */
bool ASN1_Codec::launch_producer()
{
    std::string error_string;

    producer_ptr = std::shared_ptr<RdKafka::Producer>( RdKafka::Producer::create(conf, error_string) );
    if ( !producer_ptr ) {
        elogger->critical("Failed to create producer with error: {}.", error_string );
        return false;
    }

    published_topic_ptr = std::shared_ptr<RdKafka::Topic>( RdKafka::Topic::create(producer_ptr.get(), published_topic_name, tconf, error_string) );
    if ( !published_topic_ptr ) {
        elogger->critical("Failed to create topic: {}. Error: {}.", published_topic_name, error_string );
        return false;
    } 

    ilogger->info("Producer: {} created using topic: {}.", producer_ptr->name(), published_topic_name);
    return true;
}

/**
 * JMC: asn1 reviewed.
 */
bool ASN1_Codec::launch_consumer()
{
    std::string error_string;

    consumer_ptr = std::shared_ptr<RdKafka::KafkaConsumer>( RdKafka::KafkaConsumer::create(conf, error_string) );
    if (!consumer_ptr) {
        elogger->critical("Failed to create consumer with error: {}",  error_string );
        return false;
    }

    // wait on the topics we specified to become available for subscription.
    // loop terminates with a signal (CTRL-C) or when all the topics are available.
    int tcount = 0;
    for ( auto& topic : consumed_topics ) {
        while ( data_available && tcount < consumed_topics.size() ) {
            if ( topic_available(topic) ) {
                ilogger->trace("Consumer topic: {} is available.", topic);
                // count it and attempt to get the next one if it exists.
                ++tcount;
                break;
            }
            // topic is not available, wait for a second or two.
            std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );
            ilogger->trace("Waiting for needed consumer topic: {}.", topic);
        }
    }

    if ( tcount == consumed_topics.size() ) {
        // all the needed topics are available for subscription.
        RdKafka::ErrorCode status = consumer_ptr->subscribe(consumed_topics);
        if (status) {
            elogger->critical("Failed to subscribe to {} topics. Error: {}.", consumed_topics.size(), RdKafka::err2str(status) );
            return false;
        }
    } else {
        ilogger->warn("User cancelled ASN1_Codec while waiting for topics to become available.");
        return false;
    }

    std::ostringstream osbuf{};
    for ( auto& topic : consumed_topics ) {
        if ( osbuf.tellp() != 0 ) osbuf << ", ";
        osbuf << topic;
    }

    ilogger->info("Consumer: {} created using topics: {}.", consumer_ptr->name(), osbuf.str());
    return true;
}

bool ASN1_Codec::make_loggers( bool remove_files )
{
    // defaults.
    std::string path{ "logs/" };
    std::string ilogname{ "log.info" };
    std::string elogname{ "log.error" };

    if (getOption('D').hasArg()) {
        // replace default
        path = getOption('D').argument();
        if ( path.back() != '/' ) {
            path += '/';
        }
    }

    // if the directory specified doesn't exist, then make it.
    if (!dirExists( path )) {
#ifndef _MSC_VER
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0)   // linux
#elif _MSC_VER 
        if (_mkdir(path.c_str()) != 0)                                          // windows
#else
                                                                                // some other strange os...
#endif
        {
            std::cerr << "Error making the logging directory.\n";
            return false;
        }
    }
    
    // ilog check for user-defined file locations and names.
    if (getOption('i').hasArg()) {
        // replace default.
        ilogname = string_utilities::basename<std::string>( getOption('i').argument() );
    }

    if (getOption('e').hasArg()) {
        // replace default.
        elogname = string_utilities::basename<std::string>( getOption('e').argument() );
    }
    
    ilogname = path + ilogname;
    elogname = path + elogname;

    if ( remove_files && fileExists( ilogname ) ) {
        if ( std::remove( ilogname.c_str() ) != 0 ) {
            std::cerr << "Error removing the previous information log file.\n";
            return false;
        }
    }

    if ( remove_files && fileExists( elogname ) ) {
        if ( std::remove( elogname.c_str() ) != 0 ) {
            std::cerr << "Error removing the previous error log file.\n";
            return false;
        }
    }

    // setup information logger.
    ilogger = spdlog::rotating_logger_mt("ilog", ilogname, ilogsize, ilognum);
    ilogger->set_pattern("[%C%m%d %H:%M:%S.%f] [%l] %v");
    ilogger->set_level( iloglevel );

    // setup error logger.
    elogger = spdlog::rotating_logger_mt("elog", elogname, elogsize, elognum);
    elogger->set_level( iloglevel );
    elogger->set_pattern("[%C%m%d %H:%M:%S.%f] [%l] %v");
    return true;
}

bool ASN1_Codec::decode_travelerinformation_data( std::string& data_as_hex, xer_buffer_t* xml_buffer ) {

    asn_dec_rval_t decode_rval;
    asn_enc_rval_t encode_rval;

    char errbuf[128];
    std::size_t errlen = sizeof(errbuf);

    std::vector<char> byte_buffer;

    TravelerInformation_t *travelerinformation = 0;        // must initialize to 0 according to asn.1 instructions.

    ilogger->trace("Start encode/decode operation on the input hex encoding.");

    // remove all spaces.
    data_as_hex.erase( remove_if ( data_as_hex.begin(), data_as_hex.end(), isspace), data_as_hex.end());

    if (data_as_hex.empty()) {
        elogger->warn("Input XML file payload missing payload data hex bytes!");
        return false;
    }

    ilogger->trace("Successfully extracted {} hex string: {}", asn_DEF_TravelerInformation.name, data_as_hex );

    if (!decode_hex_(data_as_hex, byte_buffer)) {
        elogger->warn("Could not decode XML payload data hex bytes!");
        return false;
    }

    ilogger->trace("Successful conversion to raw byte buffer.");

    // Decode BAH Bytes (A 1609.2 Frame) into the appropriate structure.
    decode_rval = uper_decode_complete( 
            0, 
            &asn_DEF_TravelerInformation, 
            (void **)&travelerinformation, 
            byte_buffer.data(), 
            byte_buffer.size() 
            );

    if ( decode_rval.code != RC_OK ) {
        elogger->error("Decode error for {} bytes.", asn_DEF_TravelerInformation.name );
        return false;
    }

    ilogger->trace("Successful ASN.1 Decode from bytes to C structure.");

    if (asn_check_constraints( &asn_DEF_TravelerInformation, travelerinformation, errbuf, &errlen )) {
        ASN_STRUCT_FREE(asn_DEF_TravelerInformation, travelerinformation);
        std::string m{ errbuf, errlen };
        elogger->error("{} constraint checks failed: {} this message will be dropped.", asn_DEF_TravelerInformation.name, m );
        return false;
    }

    // Encode the Ieee1609Dot2Data ASN.1 C struct into XML, so we can extract out the BSM.
    encode_rval = xer_encode( 
            &asn_DEF_TravelerInformation, 
            travelerinformation, 
            XER_F_CANONICAL, 
            xer_buffer_append, 
            static_cast<void *>(xml_buffer) 
            );

    ASN_STRUCT_FREE(asn_DEF_TravelerInformation, travelerinformation);

    if ( encode_rval.encoded == -1 ) {
        elogger->error("Cannot convert the block into XML.");
        return false;
    }

    ilogger->trace("Completed encode/decode operation on the input hex encoding; now have XML of 1609.2 frame.");
    return true;
}

bool ASN1_Codec::decode_1609dot2_data( std::string& data_as_hex, xer_buffer_t* xml_buffer, enum asn_transfer_syntax asntype ) {

    asn_dec_rval_t decode_rval;
    asn_enc_rval_t encode_rval;

    char errbuf[128];
    std::size_t errlen = sizeof(errbuf);

    std::vector<char> byte_buffer;

    Ieee1609Dot2Data_t *ieee1609data = 0;        // must initialize to 0 according to asn.1 instructions.

    ilogger->trace("Start encode/decode operation on the input hex encoding.");

    // remove all spaces.
    data_as_hex.erase( remove_if ( data_as_hex.begin(), data_as_hex.end(), isspace), data_as_hex.end());

    if (data_as_hex.empty()) {
        elogger->warn("Input XML file payload missing payload data hex bytes!");
        return false;
    }

    ilogger->trace("Successfully extracted {} hex string: {}", asn_DEF_Ieee1609Dot2Data.name, data_as_hex );

    if (!decode_hex_(data_as_hex, byte_buffer)) {
        elogger->warn("Could not decode XML payload data hex bytes!");
        return false;
    }

    ilogger->trace("Successful conversion to raw byte buffer.");

    // Decode BAH Bytes (A 1609.2 Frame) into the appropriate structure.
    // asntype is specifiedin input xml from ODE.
    decode_rval = asn_decode( 
            0, 
            asntype, 
            &asn_DEF_Ieee1609Dot2Data, 
            (void **)&ieee1609data, 
            byte_buffer.data(), 
            byte_buffer.size() 
            );

    // decode_rval = uper_decode_complete( 
    //         0, 
    //         &asn_DEF_Ieee1609Dot2Data, 
    //         (void **)&ieee1609data, 
    //         byte_buffer.data(), 
    //         byte_buffer.size() 
    //         );

    if ( decode_rval.code != RC_OK ) {
        elogger->error("Decode error for {} bytes.", asn_DEF_Ieee1609Dot2Data.name );
        return false;
    }

    ilogger->trace("Successful ASN.1 Decode from bytes to C structure.");

    if (asn_check_constraints( &asn_DEF_Ieee1609Dot2Data, ieee1609data, errbuf, &errlen )) {
        ASN_STRUCT_FREE(asn_DEF_Ieee1609Dot2Data, ieee1609data);
        std::string m{ errbuf, errlen };
        elogger->error("{} constraint checks failed: {} this message will be dropped.", asn_DEF_Ieee1609Dot2Data.name, m );
        return false;
    }

    // target form is always XML (for now).
    encode_rval = xer_encode( 
            &asn_DEF_Ieee1609Dot2Data, 
            ieee1609data, 
            XER_F_CANONICAL, 
            xer_buffer_append, 
            static_cast<void *>(xml_buffer) 
            );

    ASN_STRUCT_FREE(asn_DEF_Ieee1609Dot2Data, ieee1609data);

    if ( encode_rval.encoded == -1 ) {
        elogger->error("Cannot convert the block into XML.");
        return false;
    }

    ilogger->trace("Completed encode/decode operation on the input hex encoding; now have XML of 1609.2 frame.");
    return true;
}

/**
 * TODO: This method should be generalizable to any type def and structure pointer -- tried but moved on.
 */
// bool ASN1_Codec::test( std::string& data_as_hex, xer_buffer_t* xml_buffer, struct asn_TYPE_descriptor_s *type_desc, void **sptr ) {
bool ASN1_Codec::decode_messageframe_data( std::string& data_as_hex, xer_buffer_t* xml_buffer, enum asn_transfer_syntax atype ) {
    asn_dec_rval_t decode_rval;
    asn_enc_rval_t encode_rval;

    char errbuf[128];
    std::size_t errlen = sizeof(errbuf);

    std::vector<char> byte_buffer;

    MessageFrame_t *messageframe = 0;           // must be initialized to 0.

    ilogger->trace("Start encode/decode operation on the IEEE 1609.2 unsecuredData (J2735 MessageFrame).");

    // remove all spaces.
    data_as_hex.erase( remove_if ( data_as_hex.begin(), data_as_hex.end(), isspace), data_as_hex.end());

    if (data_as_hex.empty()) {
        elogger->warn("IEEE {} in hex has no data.", asn_DEF_MessageFrame.name );
        return false;
    }

    ilogger->trace("Successfully extracted {} hex string: {}", asn_DEF_MessageFrame.name, data_as_hex );

    if (!decode_hex_(data_as_hex, byte_buffer)) {
        elogger->warn("Could not decode XML payload data hex bytes!");
        return false;
    }

    ilogger->trace("Successful conversion to raw byte buffer.");

    decode_rval = uper_decode_complete( 
            0, 
            &asn_DEF_MessageFrame,
            (void **)&messageframe,
            byte_buffer.data(), 
            byte_buffer.size() 
            );

    if ( decode_rval.code != RC_OK ) {
        elogger->error("Decode error for {} bytes.", asn_DEF_MessageFrame.name );
        return false;
    }

    ilogger->trace("Successful ASN.1 Decode from bytes to C structure.");

    if (asn_check_constraints( &asn_DEF_MessageFrame, messageframe, errbuf, &errlen )) {
        ASN_STRUCT_FREE(asn_DEF_MessageFrame, messageframe);
        std::string m{ errbuf, errlen };
        elogger->error("{} constraint checks failed: {} this message will be dropped.", asn_DEF_MessageFrame.name, m );
        return false;
    }

    // Encode the Ieee1609Dot2Data ASN.1 C struct into XML, so we can extract out the BSM.
    encode_rval = xer_encode( 
            &asn_DEF_MessageFrame, 
            messageframe, 
            XER_F_CANONICAL, 
            xer_buffer_append, 
            static_cast<void *>(xml_buffer) 
            );

    ASN_STRUCT_FREE(asn_DEF_MessageFrame, messageframe);

    if ( encode_rval.encoded == -1 ) {
        elogger->error("Cannot convert the block into XML.");
        return false;
    }

    ilogger->trace("Completed ASN.1 encode/decode to final XML form.");
    return true;
}

/**
 * Used as a test stub to bypass Kafka and work through the parsing, encoding, decoding.
 *
 */
bool ASN1_Codec::filetest() {

    std::string error_string;
    RdKafka::ErrorCode status;

    // flags for type of decoding required.
    bool decode_1609dot2 = false;
    bool decode_messageframe = false;
    enum asn_transfer_syntax decode_1609dot2_type = ATS_CANONICAL_OER;
    enum asn_transfer_syntax decode_messageframe_type = ATS_UNALIGNED_BASIC_PER;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    
    try {

        // throws for a couple of options.
        if ( !configure() ) return EXIT_FAILURE;

    } catch ( std::exception& e ) {

        // don't use logger in case we cannot configure it correctly.
        std::cerr << "Fatal Exception: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // read in BAH test input file.
    std::FILE* ifile = std::fopen( operands[0].c_str(), "r" );
    if (!ifile) {
        std::cerr << "cannot open " << operands[0] << '\n';
        return EXIT_FAILURE;
    }

    // compute file size in bytes.
    std::fseek(ifile, 0, SEEK_END);
    std::size_t ifile_size = std::ftell(ifile);
    std::fseek(ifile, 0, SEEK_SET);

    // read bytes from file into appropriately sized consumed_xml_buffer.
    std::vector<unsigned char> consumed_xml_buffer( ifile_size );
    std::size_t count = std::fread( consumed_xml_buffer.data(), sizeof( uint8_t ), consumed_xml_buffer.size(), ifile );
    std::fclose( ifile );

    if ( consumed_xml_buffer.size() > 0 ) {

        ilogger->trace("Read the test file having {} bytes.", consumed_xml_buffer.size() );

        msg_recv_count++;
        msg_recv_bytes += consumed_xml_buffer.size();

        xer_buffer_t xb = {0, 0, 0};

        // Load BAH XML Input Document.
        pugi::xml_document input_doc{};
        pugi::xml_parse_result result = input_doc.load_buffer((const void*) consumed_xml_buffer.data(), consumed_xml_buffer.size(), xml_parse_options );

        if (!result) {
            ilogger->trace("Error parsing consumed XML file: {} (offset = {})!", result.description(), result.offset);
            return EXIT_FAILURE; 
        } 

        // Determine which decodings are needed.
        pugi::xpath_node encodings_xpath_node = ode_encodings_query.evaluate_node( input_doc );

        enum asn_transfer_syntax atstype = ATS_INVALID;

        for ( pugi::xml_node n = encodings_xpath_node.node().first_child(); n; n = n.next_sibling()) {

            pugi::xml_text ats_node = n.child("encodingRule").text();
            if ( ats_node ) {
                atstype = get_ats_transfer_syntax( ats_node.get() );
            }

            ilogger->trace("Inside the loop.");
            if ( std::strcmp(n.child("elementType").text().get(), "Ieee1609Dot2Data") == 0 ) {
                decode_1609dot2 = true;
                if ( atstype != ATS_INVALID ) {
                    // only change the default values if a new type is provided in the XML.
                    decode_1609dot2_type = atstype;
                }
                ilogger->trace("Setting the decode 1609.2 flag.");
            } else if ( std::strcmp(n.child("elementType").text().get(), "MessageFrame") == 0 ) {
                decode_messageframe = true;
                if ( atstype != ATS_INVALID ) {
                    // only change the default values if a new type is provided in the XML.
                    decode_messageframe_type = atstype;
                }
                ilogger->trace("Setting the decode message frame flag.");
            }
        }

        pugi::xpath_node payload_xpath_node = ode_payload_query.evaluate_node( input_doc );

        // Retain this node reference. It is where the decoded result will be inserted.
        pugi::xml_node payload_node = payload_xpath_node.node();

        // access this directly because we remove the bytes branch.
        pugi::xml_text text = payload_node.child("bytes").text();

        if ( !text ) {
            elogger->warn("No text node (data source) in the consumed XML file.");
            return EXIT_FAILURE;
        }

        std::string hstr{ text.get() };

        // Declaration needed outside of the scope of the below conditionals.
        // Reuse since we do not need to retain the outer frame (if required).
        pugi::xml_document internal_doc{};

        // first must be 1609dot2
        if ( decode_1609dot2 ) {

            // TODO Refactor: input = hstr, decode type; output = hstr (or EXIT_FAILURE)
            if ( decode_1609dot2_data(hstr, &xb, decode_1609dot2_type ) == false ) {
                elogger->warn("Failure to extract/convert consumed XML file payload into a byte buffer.");
                return EXIT_FAILURE;
            }

            //pugi::xml_document ieee_doc{};
            result = internal_doc.load_buffer(static_cast<const void *>( xb.buffer), xb.buffer_size );

            std::free( static_cast<void *>(xb.buffer) );

            if (!result) {
                ilogger->trace("Error parsing IEEE 1609.2 XML data: {} (offset = {})!", result.description(), result.offset);
                return EXIT_FAILURE; 
            } 

            // reset the buffer.
            xb = { 0,0,0 };

            // XPath search the IEEE structure for the unsecured data.
            pugi::xpath_node unsecuredDataNode = ieee1609dot2_unsecuredData_query.evaluate_node( internal_doc );
            text = unsecuredDataNode.node().text();

            if ( !text ) {
                elogger->warn("No text node (data source) found in the IEEE 1609.2 XML document.");
                return EXIT_FAILURE;
            }

            // replacing the original hex string, so the next processing step works.
            hstr = std::string( text.get() );
        }

        internal_doc.reset();

        if ( decode_messageframe ) {

            if ( decode_messageframe_data( hstr, &xb, decode_messageframe_type ) == false ) {
                elogger->warn("Failure to extract/convert IEEE 1609.2 XML file payload into a byte buffer.");
                return EXIT_FAILURE;
            }

            ilogger->info( "IEEE 1609.2 Unsecured Data Field Payload decode/encode operation complete (usually a J2735 MessageFrame)." );

            // zero out hex string.
            payload_node.text().set("");

            // build an XML document from the ascii data.
            // pugi::xml_document messageframexml{};
            result = internal_doc.load_buffer( static_cast<const void *>( xb.buffer), xb.buffer_size );
            std::free( static_cast<void *>(xb.buffer) );

            if (!result) {
                ilogger->trace("Error parsing J2735 XML data: {} (offset = {})!", result.description(), result.offset);
                return EXIT_FAILURE; 
            } 

        }

        // replace the hex data with XML data.
        payload_node.remove_child("bytes");

        if ( internal_doc ) {
            // TODO: make sure this is checking for empty document cases.
            payload_node.append_copy( internal_doc.document_element() );
        } else {  // else doc the remaining document should be empty now!
            ilogger->trace("Internal doc is empty.");
        }

        // convert DOM to a RAW string representation: no spaces, no tabs.
        std::stringstream ss;
        input_doc.save(ss,"",pugi::format_raw);
        std::cout << ss.str() << "\n";
        return true;

    } else {
        ilogger->trace("Read an empty file.");
    }

    // NOTE: good for troubleshooting, but bad for performance.
    elogger->flush();
    ilogger->flush();

    return true;
}

int ASN1_Codec::operator()(void) {

    std::string error_string;
    std::stringstream xmlss;

    RdKafka::ErrorCode status;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    
    try {

        // throws for a couple of options.
        if ( !configure() ) return EXIT_FAILURE;

    } catch ( std::exception& e ) {

        // don't use logger in case we cannot configure it correctly.
        std::cerr << "Fatal Exception: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if ( !launch_consumer() ) return EXIT_FAILURE;
    if ( !launch_producer() ) return EXIT_FAILURE;

    // consume-produce loop.
    while (data_available) {

        std::unique_ptr<RdKafka::Message> msg{ consumer_ptr->consume( consumer_timeout ) };

        if ( msg->len() > 0 && msg_consume(msg.get(), xmlss) ) {


            std::cerr << msg->len() << " bytes consumed from topic: " << consumed_topics[0] << '\n';

            std::string xml_output = xmlss.str();
            status = producer_ptr->produce(published_topic_ptr.get(), partition, RdKafka::Producer::RK_MSG_COPY, (void *)xml_output.c_str(), xml_output.size(), NULL, NULL);

            if (status != RdKafka::ERR_NO_ERROR) {
                elogger->error("Failure of XER encoding: {}", RdKafka::err2str( status ));

            } else {
                // successfully sent; update counters.
                msg_send_count++;
                msg_send_bytes += xml_output.size();
                ilogger->trace("Success of XER encoding.");
                std::cerr << xml_output.size() << " bytes produced to topic: " << published_topic_ptr->name() << '\n';
            }

            // clear out the stream
            xmlss.str("");
            xmlss.clear();
        } 

        // NOTE: good for troubleshooting, but bad for performance.
        elogger->flush();
        ilogger->flush();
    }

    ilogger->info("ASN1_Codec operations complete; shutting down...");
    ilogger->info("ASN1_Codec consumed  : {} blocks and {} bytes", msg_recv_count, msg_recv_bytes);
    ilogger->info("ASN1_Codec published : {} blocks and {} bytes", msg_send_count, msg_send_bytes);

    std::cerr << "ASN1_Codec operations complete; shutting down...\n";
    std::cerr << "ASN1_Codec consumed   : " << msg_recv_count << " blocks and " << msg_recv_bytes << " bytes\n";
    std::cerr << "ASN1_Codec published  : " << msg_send_count << " blocks and " << msg_send_bytes << " bytes\n";
    return EXIT_SUCCESS;
}

bool ASN1_Codec::decode_hex_(const std::string& payload_hex, std::vector<char>& byte_buffer) {
    uint8_t d = 0;
    int i = 0;          // so we can return -1;

    for (const char& c : payload_hex) {
        if ( c <= '9' && c >= '0' ) {
            d = c-'0';
        } else if ( c <= 'F' && c >= 'A' ) {
            d = c-55;       // c - 'A' + 10
        } else if ( c <= 'f' && c >= 'a' ) {
            d = c-87;        // c - 'a' + 10;
        } else {
            return false;
        }

        if (i%2) {
            // low order nibble.
            byte_buffer.back() |= d;
        } else {
            // high order nibble.
            byte_buffer.push_back( d<<4 );
        }
        ++i;
    }

    // the number of bytes in the buf.
    return true;
}

#ifndef _ASN1_CODEC_TESTS

int main( int argc, char* argv[] )
{
    ASN1_Codec asn1_codec{"ASN1_Codec","ASN1 Processing Module"};

    asn1_codec.addOption( 'c', "config", "Configuration file name and path.", true );
    asn1_codec.addOption( 'C', "config-check", "Check the configuration file contents and output the settings.", false );
    asn1_codec.addOption( 't', "produce-topic", "The name of the topic to produce.", true );
    asn1_codec.addOption( 'p', "partition", "Consumer topic partition from which to read.", true );
    asn1_codec.addOption( 'g', "group", "Consumer group identifier", true );
    asn1_codec.addOption( 'b', "broker", "Broker address (localhost:9092)", true );
    asn1_codec.addOption( 'o', "offset", "Byte offset to start reading in the consumed topic.", true );
    asn1_codec.addOption( 'x', "exit", "Exit consumer when last message in partition has been received.", false );
    asn1_codec.addOption( 'd', "debug", "debug level.", true );
    asn1_codec.addOption( 'v', "log-level", "The info log level [trace,debug,info,warning,error,critical,off]", true );
    asn1_codec.addOption( 'D', "log-dir", "Directory for the log files.", true );
    asn1_codec.addOption( 'R', "log-rm", "Remove specified/default log files if they exist.", false );
    asn1_codec.addOption( 'i', "ilog", "Information log file name.", true );
    asn1_codec.addOption( 'e', "elog", "Error log file name.", true );
    asn1_codec.addOption( 'h', "help", "print out some help" );
    asn1_codec.addOption( 'F', "infile", "accept a file and bypass kafka.", false );


    // debug for ASN.1
    // opt_debug = 0;

    if (!asn1_codec.parseArgs(argc, argv)) {
        asn1_codec.usage();
        std::exit( EXIT_FAILURE );
    }

    if (asn1_codec.optIsSet('h')) {
        asn1_codec.help();
        std::exit( EXIT_SUCCESS );
    }

    // can set levels if needed here.
    if ( !asn1_codec.make_loggers( asn1_codec.optIsSet('R') )) {
        std::exit( EXIT_FAILURE );
    }

    // configuration check.
    if (asn1_codec.optIsSet('C')) {
        try {
            if (asn1_codec.configure()) {
                asn1_codec.print_configuration();
                std::exit( EXIT_SUCCESS );
            } else {
                std::cerr << "Current configuration settings do not work.\n";
                asn1_codec.ilogger->error( "current configuration settings do not work; exiting." );
                std::exit( EXIT_FAILURE );
            }
        } catch ( std::exception& e ) {
            std::cerr << "Fatal Exception: " << e.what() << '\n';
            asn1_codec.elogger->error( "exception: {}", e.what() );
            std::exit( EXIT_FAILURE );
        }
    }

    if (asn1_codec.optIsSet('F')) {
        // Use a file for testing and bypass Kafka; file is the first operand.
        std::exit( asn1_codec.filetest() );
    } else {
        // The module will run and when it terminates return an appropriate error code.
        std::exit( asn1_codec.run() );
    }
}

#endif
