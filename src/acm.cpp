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

#include "acm.hpp"
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
static int dynamic_buffer_append(const void *buffer, size_t size, void *app_key) {
    buffer_structure_t *xb = static_cast<buffer_structure_t *>(app_key);

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
bool ASN1_Codec::bootstrap = true;

const char* asn1errortypes[] = {
    [static_cast<int>(Asn1ErrorType::SUCCESS)] = "SUCCESS",
    [static_cast<int>(Asn1ErrorType::FAILURE)] = "FAILURE",
    [static_cast<int>(Asn1ErrorType::REQUEST)] = "INVALID_REQUEST_TYPE_ERROR",
    [static_cast<int>(Asn1ErrorType::DATA)]    = "INVALID_DATA_TYPE_ERROR"
};

const char* asn1datatypes[] = {
    [static_cast<int>(Asn1DataType::ODE)] = "us.dot.its.jpo.ode.model.OdeStatus",
    [static_cast<int>(Asn1DataType::XML)] = "MessageFrame",
    [static_cast<int>(Asn1DataType::HEX)] = "us.dot.its.jpo.ode.model.OdeHexByteArray",
    [static_cast<int>(Asn1DataType::PAYLOAD)] = "us.dot.its.jpo.ode.model.OdeAsn1Payload"
};

std::ostream& operator<<( std::ostream& os, Asn1ErrorType err ) {
    os << asn1errortypes[static_cast<int>(err)];
	return os;
}

std::ostream& operator<<( std::ostream& os, Asn1DataType dt ) {
    os << asn1datatypes[static_cast<int>(dt)];
	return os;
}

ASN1_Codec::ASN1_Codec( const std::string& name, const std::string& description ) :
    Tool{ name, description }
    , exit_eof{true}
    , eof_cnt{0}
    , partition_cnt{1}
    , msg_recv_count{0}
    , msg_send_count{0}
    , msg_filt_count{0}
    , msg_recv_bytes{0}
    , msg_send_bytes{0}
    , msg_filt_bytes{0}
    , iloglevel{ spdlog::level::trace }
    , eloglevel{ spdlog::level::err }
    , pconf{}
    , brokers{"localhost"}
    , partition{RdKafka::Topic::PARTITION_UA}
    , mode{""}
    , debug{""}
    , consumed_topics{}
    , offset{RdKafka::Topic::OFFSET_BEGINNING}
    , published_topic_name{}
    , conf{nullptr}
    , tconf{nullptr}
    , consumer_ptr{}
    , consumer_timeout{500}
    , producer_ptr{}
    , published_topic_ptr{}
    , input_doc{}
    , internal_doc{}
    , error_doc{}
    , xml_parse_options{ pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata }
    , ieee1609dot2_unsecuredData_query{"Ieee1609Dot2Data/content//unsecuredData"}  // this will work on both signed and unsigned
    , ode_payload_query{"OdeAsn1Data/payload/data"}
    , ode_encodings_query{"OdeAsn1Data/metadata/encodings"}
    , erroross{}
    , byte_buffer{}
	, opsflag{0}
    , decode_1609dot2{ false }
    , decode_messageframe{ false }
	, decode_asdframe{ false }
	, decode_functionality{ true }
	, decode_1609dot2_type{ATS_CANONICAL_OER}
	, decode_messageframe_type{ATS_UNALIGNED_BASIC_PER}
	, decode_asdframe_type{ATS_UNALIGNED_BASIC_PER}
    , ilogger{}
    , elogger{}
{
}

ASN1_Codec::~ASN1_Codec() 
{
    if (consumer_ptr) {
        consumer_ptr->close();
    }

    // free raw librdkafka pointers.
    if (tconf) delete tconf;
    if (conf) delete conf;

    // TODO: This librdkafka item seems wrong...
    RdKafka::wait_destroyed(5000);    // pause to let RdKafka reclaim resources.
}

std::string ASN1_Codec::get_current_time() const {
	char buf[50];
	std::time_t t = std::time(NULL);

	if ( std::strftime(buf, sizeof(buf), "%Y-%m-%dT%TZ[UTC]", std::gmtime(&t) ) ) {
		return std::string{ buf };
	}

	return std::string{};
}

void ASN1_Codec::sigterm (int sig) {
    data_available = false;
    bootstrap = false;
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

bool ASN1_Codec::configure() {

    static const char* fnname = "configure()";
    std::string line;
    std::string error_string;
    StrVector pieces;

    ilogger->trace("{}: starting...", fnname );

    // configurations; global and topic (the names in these are fixed)
    conf  = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // must use a configuration file.
    if ( !optIsSet('c') ) {
        std::cout << fnname << ": asked to use a configuration file, but option not set.\n";
        elogger->error( "{}: asked to use a configuration file, but option not set.", fnname  );
        return false;
    }

    const std::string& cfile = optString('c');              // needed for error message.
    ilogger->trace("{}: using configuration file: {}", fnname , cfile );
    std::ifstream ifs{ cfile };

    if (!ifs) {
        std::cout << fnname << ": cannot open configuration file: " << cfile << '\n';
        elogger->error("{}: cannot open configuration file: {}", fnname , cfile);
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
                    ilogger->info("{}: kafka topic configuration: {} = {}", fnname , pieces[0], pieces[1]);
                    done = true;
                }

                if ( conf->set(pieces[0], pieces[1], error_string) == RdKafka::Conf::CONF_OK ) {
                    ilogger->info("{}: kafka configuration: {} = {}", fnname , pieces[0], pieces[1]);
                    done = true;
                }

                if ( !done ) { 
                    ilogger->info("{}: ASN1_Codec configuration: {} = {}", fnname , pieces[0], pieces[1]);
                    // These configuration options are not expected by Kafka.
                    // Assume there are for the ASN1_Codec.
                    pconf[ pieces[0] ] = pieces[1];
                }

            } else {
                elogger->warn("{}: too many pieces in the configuration file line: {}", fnname , line);
            }

        } // otherwise: empty or comment line.
    }

    // All configuration file settings are overridden, if supplied, by CLI options. Those occur here.
    
    // decoder or encoder needed? decoder is the default and decode_functionality is already true.
    // only change from default if the user CORRECTLY specified what to use.
    auto search = pconf.find("acm.type");
    if ( search != pconf.end() ) {
        if ( "encode" == search->second ) decode_functionality = false;
        else if ( "decode" == search->second ) decode_functionality = true;
    }  

    if (optIsSet('T')) {
        if ( "encode" == optString('T') ) decode_functionality = false;
        else if ( "decode" == optString('T') ) decode_functionality = true;
    } 
    
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

    std::string errorfile{"./config/Output.error.xml"};

    search = pconf.find("acm.error.template");
    if ( search != pconf.end() ) {
        errorfile = search->second;
    }  

    pugi::xml_parse_result result = error_doc.load_file( errorfile.c_str() );
    if (!result) {
        elogger->error("{}: Failure to find or parse the error template file: {} (offset = {})!", fnname , result.description(), result.offset);
        return false;
    } 

    if ( optIsSet('b') ) {
        // broker specified.
        ilogger->info("{}: setting kafka broker to: {}", fnname , optString('b'));
        conf->set("metadata.broker.list", optString('b'), error_string);
    } 

    if ( optIsSet('p') ) {
        // number of partitions.
        partition = optInt( 'p' );

    } else {
        auto search = pconf.find("asn1.kafka.partition");
        if ( search != pconf.end() ) {
            partition = std::stoi(search->second);              // throws.    
        }  // otherwise leave at default; PARTITION_UA
    }

    ilogger->info("{}: kafka partition: {}", fnname , partition);

    if ( getOption('g').isSet() && conf->set("group.id", optString('g'), error_string) != RdKafka::Conf::CONF_OK) {
        // NOTE: there are some checks in librdkafka that require this to be present and set.
        elogger->error("{}: kafka error setting configuration parameters group.id h: {}", fnname , error_string);
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

        ilogger->info("{}: offset in partition set to byte: {}", fnname , o);
    }

    // Do we want to exit if a stream eof is sent.
    exit_eof = getOption('x').isSet();

    if (optIsSet('d') && conf->set("debug", optString('d'), error_string) != RdKafka::Conf::CONF_OK) {
        elogger->error("{}: kafka error setting configuration parameter debug: {}", fnname , error_string);
        return false;
    }

    // librdkafka defined configuration.
    conf->set("default_topic_conf", tconf, error_string);

    search = pconf.find("asn1.topic.consumer");
    if ( search != pconf.end() ) {
        consumed_topics.push_back( search->second );
        ilogger->info("{}: consumed topic: {}", fnname , search->second);

    } else {
        
        elogger->error("{}: no consumer topic was specified; must fail.", fnname );
        return false;
    }

    if (optIsSet('t')) {
        // this is the produced (filtered) topic.
        published_topic_name = optString( 't' );

    } else {
        // maybe it was specified in the configuration file.
        auto search = pconf.find("asn1.topic.producer");
        if ( search != pconf.end() ) {
            published_topic_name = search->second;
        } else {
            elogger->error("{}: no publisher topic was specified; must fail.", fnname );
            return false;
        }
    }

    ilogger->info("{}: published topic: {}", fnname , published_topic_name);

    search = pconf.find("asn1.consumer.timeout.ms");
    if ( search != pconf.end() ) {
        try {
            consumer_timeout = std::stoi( search->second );
        } catch( std::exception& e ) {
            ilogger->info("{}: using the default consumer timeout value.", fnname );
        }
    }

    ilogger->trace("{}: finished.", fnname );
    return true;
}

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

    /**
     * Update the error information in the doc provided. The doc will have to conform to the schema below or this method
     * will not do anything.
     *
     *
     *
     *
     *
     * Modify the following parts:
     *      <?xml version="1.0"?>
     *      <OdeAsn1Data>
     *        <metadata>
     *          <payloadType>us.dot.its.jpo.ode.model.OdeAsn1Payload</payloadType>
     *          <serialId>
     *            <streamId></streamId>
     *            <bundleSize></bundleSize>
     *            <bundleId></bundleId>
     *            <recordId></recordId>
     *            <serialNumber></serialNumber>
     *          </serialId>
     * >>>         <receivedAt>[TIMESTAMP]</receivedAt>
     *          <schemaVersion>2</schemaVersion>
     * >>>         <generatedAt>[TIMESTAMP]</generatedAt>
     *          <logFileName></logFileName>
     *          <validSignature></validSignature>
     *          <sanitized></sanitized>
     *          <encodings>
     *          </encodings>
     *        </metadata>
     *        <payload>
     * >>>         <dataType>us.dot.its.jpo.ode.model.OdeStatus</dataType>
     *          <data>
     * >>>             <code></code>
     * >>>             <message></message>
     *          </data>
     *        </payload>
     *      </OdeAsn1Data>
     *
     *
     * doc:
     * dt:
     * et:
     * message: this string will be COPIED INTO the xml dom by pugixml.
     */

bool ASN1_Codec::add_error_xml( pugi::xml_document& doc, Asn1DataType dt, Asn1ErrorType et, std::string message, bool update_time ) {

	static const char* fnname = "add_error_xml()";
	bool r = true;

	// Attempt to set all these fields; log the errors; return false if any fail.

	// access this directly because we remove the bytes branch.
	pugi::xml_node metadata_node = doc.child("OdeAsn1Data").child("metadata");
	if ( !metadata_node ) {
		//elogger->error("{}: Cannot find OdeAsn1Data/metadata nodes in function input document.", fnname );
		return false;
	}

	pugi::xml_node payload_node  = doc.child("OdeAsn1Data").child("payload");
	if ( !payload_node ) {
		//elogger->error("{}: Cannot find OdeAsn1Data/payload nodes in function input document.", fnname );
		return false;
	}

	if ( !metadata_node.child("payloadType").text().set( asn1datatypes[static_cast<int>(Asn1DataType::PAYLOAD)]  ) ) {
		//elogger->error("{}: Failure to update the payloadType field of the error xml", fnname );
		r = false;
	}

	// receivedAt is only updatable if update_time is true.
	if ( update_time && !metadata_node.child("receivedAt").text().set( get_current_time().c_str() ) ) {
		//elogger->error("{}: Failure to update the receivedAt field of the error xml", fnname );
		r = false;
	}

	// generateAt time is always updated; it is the time of generating this message.
	if ( !metadata_node.child("generatedAt").text().set( get_current_time().c_str() ) ) {
		//elogger->error("{}: Failure to update the generatedAt field of the error xml", fnname );
		r = false;
	}

	if ( !payload_node.child("dataType").text().set( asn1datatypes[static_cast<int>(dt)]  ) ) {
		//elogger->error("{}: Failure to update the dataType field of the error xml", fnname );
		r = false;
	}

	pugi::xml_node data_node = payload_node.child("data");

	// when bytes doesn't exist this is effectively a noop.
	bool result = data_node.remove_child("bytes");

	if ( !data_node.child("code") ) {
		data_node.append_child("code");
	}

	if ( !data_node.child("message") ) {
		data_node.append_child("message");
	}

	if ( !data_node.child("code").text().set( asn1errortypes[static_cast<int>(et)]  ) ) {
		//elogger->error("{}: Failure to update the data/code field of the error xml", fnname );
		r = false;
	}

	if ( !data_node.child("message").text().set( message.c_str() ) ) {
		//elogger->error("{}: Failure to update the data/message field of the error xml", fnname );
		r = false;
	}

	return r;
}

bool ASN1_Codec::hex_to_bytes_(const std::string& payload_hex, std::vector<char>& buf) {
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
            buf.back() |= d;
        } else {
            // high order nibble.
            buf.push_back( d<<4 );
        }
        ++i;
    }

    // the number of bytes in the buf.
    return true;
}

bool ASN1_Codec::bytes_to_hex_(buffer_structure_t* buf_struct, std::string& hex_vector ) {
    char c = 0;

    hex_vector.clear();
    for ( std::size_t i = 0; i < buf_struct->buffer_size; ++i ) {

        uint8_t bh = (buf_struct->buffer[i] & 0xF0) >> 4;

        if ( bh <= 9 && bh >= 0 ) {
            c = (bh+48); // b + '0'
        } else if ( bh <= 15 && bh >= 10 ) {
            c = (bh+55); // b + 'A' - 10;
        } else {
            return false;
        }

        hex_vector.push_back(c);

        uint8_t bl = (buf_struct->buffer[i] & 0x0F);

        if ( bl <= 9 && bl >= 0 ) {
            c = (bl+48);
        } else if ( bl <= 15 && bl >= 10 ) {
            c = (bl+55);
        } else {
            return false;
        }

        hex_vector.push_back(c);
    }

    return true;
}

enum asn_transfer_syntax ASN1_Codec::get_ats_transfer_syntax( const char* ats ) {

    enum asn_transfer_syntax r = ATS_INVALID;

    if ( std::strcmp( ats, "UPER" ) == 0 ) {

        r = ATS_UNALIGNED_BASIC_PER;

    } else if ( std::strcmp( ats, "COER" ) == 0 ) {

        r = ATS_CANONICAL_OER;

    } else if ( std::strcmp( ats, "XER" ) == 0 ) {

        r = ATS_BASIC_XER;

    } else if ( std::strcmp( ats, "CPER" ) == 0 ) {

        r = ATS_UNALIGNED_CANONICAL_PER;

    } else if ( std::strcmp( ats, "CXER" ) == 0 ) {
        
        r = ATS_CANONICAL_XER;

    } else if ( std::strcmp( ats, "BER" ) == 0 ) {

        r = ATS_BER;

    } else if ( std::strcmp( ats, "DER" ) == 0 ) {

        r = ATS_DER;

    } else if ( std::strcmp( ats, "CER" ) == 0 ) {

        r = ATS_CER;

    }

    return r;
}

bool ASN1_Codec::process_message(RdKafka::Message* message, std::stringstream& output_message_stream ) {

    static const char* fnname = "process_message()";
    static std::string tsname;
    static RdKafka::MessageTimestamp ts;
    
    // flags for type of decoding required.
    pugi::xml_parse_result parse_result;

    size_t bytes_processed = 0;

	ilogger->trace("{}: starting...", fnname);

    switch (message->err()) {

        case RdKafka::ERR__TIMED_OUT:
            ilogger->info("{}: Waiting for more BSMs.", fnname );
            break;

        case RdKafka::ERR__MSG_TIMED_OUT:
            ilogger->info("{}: Waiting for more BSMs from the ODE producer.", fnname );
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            msg_recv_count++;
            msg_recv_bytes += message->len();

            ilogger->trace("{}: Read message at byte offset: {} with length {}", fnname , message->offset(), message->len() );

            ts = message->timestamp();

            if (ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
                if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME) {
                    tsname = "create time";
                } else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME) {
                    tsname = "log append time";
                } else {
                    tsname = "unknown";
                }

                ilogger->trace("{}: Message timestamp: {}, type: {}", fnname , tsname, ts.timestamp);
            }

            if ( message->key() ) {
                ilogger->trace("{}: Message key: {}", fnname , *message->key() );
            }

            // already verified non-zero message length.

            parse_result = input_doc.load_buffer((const void*) message->payload(), message->len(), xml_parse_options );

            if (!parse_result) {
                erroross.str("");
                erroross << "Input file parse error: " << parse_result.description() << " at offset " << parse_result.offset;
                throw UnparseableInputError{ erroross.str() };
            } 

            // examine the input xml encodings information and set the flags and requirements needed to properly parse
            // the byte strings.
            set_codec_requirements( input_doc );        // throws UnparseableInputErrors

            payload_node_ = ode_payload_query.evaluate_node( input_doc ).node();

            if ( !payload_node_ ) {
                throw UnparseableInputError{ "Failed to find the OdeAsn1Data/payload/data field in the input file." };
            }

            if ( decode_functionality ) {
                decode_message( payload_node_, output_message_stream );          // throws
            } else {
                encode_message( output_message_stream );          // throws
            }
                
            return true;
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

	ilogger->trace("{}: finished...", fnname);
    return false;
}

bool ASN1_Codec::decode_message( pugi::xml_node& payload_node, std::stringstream& output_message_stream ) {

    static const char* fnname = "decode_message()";
    bool success = true;
    pugi::xml_parse_result parse_result;

    buffer_structure_t xb = {0, 0, 0};

    ilogger->trace("{}: starting...", fnname);

    if ( !decode_1609dot2 && !decode_messageframe ) {
        // if neither of these is set, this function becomes a noop and nothing will be returned, so this is an
        // exception.
        throw MissingInputElementError{"An decoder was not specified in the encodingType tag that this module understands."};
    }

    // access this directly because we remove the bytes branch.
    pugi::xml_text text = payload_node.child("bytes").text();

    if ( text ) {
        // store the bytes and remove the bytes node since we replace it.
        std::string hstr{ text.get() };
        payload_node.remove_child("bytes");

        // Ieee 1609.2 is the outer frame.
		if ( decode_1609dot2 ) {

			decode_1609dot2_data(hstr, &xb);            // throws.

			// asssert success == true;

			// pugi resets the document as part of load_buffer
			parse_result = internal_doc.load_buffer(static_cast<const void *>( xb.buffer), xb.buffer_size );

			if ( !parse_result ) {
				erroross.str("");
				erroross << "IEEE 1609.2 decoded XER cannot be parsed/loaded as a valid document: " << parse_result.description() << " at offset " << parse_result.offset;
				throw Asn1CodecError{ erroross.str() };
			}

			// XPath search the IEEE structure for the unsecured data.
			pugi::xpath_node unsecuredDataNode = ieee1609dot2_unsecuredData_query.evaluate_node( internal_doc );
			text = unsecuredDataNode.node().text();

			if ( !text ) throw Asn1CodecError{"IEEE 1609.2 internal XER unsecuredData element could not be found."};

			// replacing the original hex string, so the next processing step works.
			hstr = std::string( text.get() );
			internal_doc.reset();
			std::free( static_cast<void *>(xb.buffer) );
			xb = { 0,0,0 };                     // reset buffer;
		}

		if ( success && decode_messageframe ) {

			decode_messageframe_data( hstr, &xb );          // throws.

			// asssert success == true;

			// eliminate the original hex string, so the new XML can be inserted.
			payload_node.text().set("");
			parse_result = internal_doc.load_buffer( static_cast<const void *>( xb.buffer), xb.buffer_size );

			if ( !parse_result ) {
				erroross.str("");
				erroross <<"J2735 decoded XER cannot be parsed/loaded as a valid document: "<< parse_result.description() << " at offset " << parse_result.offset;
				throw Asn1CodecError{ erroross.str() };
			}

			payload_node.append_copy( internal_doc.document_element() );

			if ( !payload_node.parent().child("dataType").text().set( asn1datatypes[static_cast<int>(Asn1DataType::XML)] ) ) {
				throw MissingInputElementError{"Could not update the dataType field of the payload section."};
			}

			std::free( static_cast<void *>(xb.buffer) );
		}

    } else {
        throw MissingInputElementError{"failure accessing input XML bytes node."};
    }

    // convert DOM to a RAW string representation: no spaces, no tabs.
    input_doc.save(output_message_stream,"",pugi::format_raw);
    ilogger->trace("{}: finished...", fnname);
    return success;
} 

void ASN1_Codec::encode_node_as_hex_string(bool replace) {
    std::stringstream xml_stream;
    std::string hex_str;

    pugi::xml_node node = payload_node_.first_element_by_path(curr_node_path_.c_str());

    if (!node) {
        throw MissingInputElementError{"Failed to find path: " + curr_node_path_ + "in the input document."};
    }

    pugi::xml_node parent_node = node.parent();

    if (!parent_node) {
        throw MissingInputElementError{"Failed to find parent node for: " + curr_node_path_ + "in the input document."};
    }

    // convert the child to string stream 
    node.print(xml_stream, "", pugi::format_raw);

    // remove the child node from parent
    if ( !parent_node.remove_child(node) ) {
        throw MissingInputElementError{"Failed to find child node in the input document."};
    }

    // do the encoding
    encode_frame_data(xml_stream.str(), hex_str);

    std::string node_name(node.name());
    hex_data_.push_back(std::make_tuple(node_name, hex_str));

    if (!replace) {
        return;
    }

    // append the hex bytes as a new node
    if ( !parent_node.text().set(hex_str.c_str()) ) {
        throw MissingInputElementError{"Failure to append hex bytes to the output document."};
    }
}

void ASN1_Codec::encode_for_protocol() {
    for (auto& part : protocol_) {
        curr_op_ = std::get<0>(part);
        curr_decode_type_ = std::get<1>(part);
        curr_node_path_ = std::get<2>(part);

        encode_node_as_hex_string(std::get<3>(part));
    }

    for (auto& data : hex_data_) {
        std::string node_name = std::get<0>(data);
        std::string hex_str = std::get<1>(data);

        if ( !payload_node_.append_child(node_name.c_str()).append_child("bytes").text().set(hex_str.c_str()) ) {
            throw MissingInputElementError{"Failure to append path: OdeAsn1Data/payload/data/" + node_name + "/bytes to the output document."};
        }
    }

    if (!payload_node_.parent().child("dataType").text().set( asn1datatypes[static_cast<int>(Asn1DataType::HEX)] ) ) {
            throw MissingInputElementError{"Failure to update path: OdeAsn1Data/payload/dataType in the output document."};
    }
}

// throws MissingInputElementError or Asn1CodecError (from encode_messageframe_data call) ONLY!
bool ASN1_Codec::encode_message( std::stringstream& output_message_stream ) {

    static const char* fnname = "encode_message()";

    protocol_.clear();
    hex_data_.clear();

    switch (opsflag) {
        case IEEE1609DOT2:
            protocol_.push_back(std::make_tuple(IEEE1609DOT2, decode_1609dot2_type, "Ieee1609Dot2Data", false));

            break;
        case J2735MESSAGEFRAME:
            protocol_.push_back(std::make_tuple(J2735MESSAGEFRAME, decode_messageframe_type, "MessageFrame", false));

            break;
        case IEEE1609DOT2_J2735MESSAGEFRAME:
            protocol_.push_back(std::make_tuple(J2735MESSAGEFRAME, decode_messageframe_type, "Ieee1609Dot2Data/content/unsecuredData/MessageFrame", true));
            protocol_.push_back(std::make_tuple(IEEE1609DOT2, decode_1609dot2_type, "Ieee1609Dot2Data", false));

            break;
        case ASDFRAME:
            protocol_.push_back(std::make_tuple(ASDFRAME, decode_asdframe_type, "AdvisorySituationData", false));

            break;
        case ASDFRAME_IEEE1609DOT2:
            protocol_.push_back(std::make_tuple(IEEE1609DOT2, decode_1609dot2_type, "AdvisorySituationData/asdmDetails/advisoryMessage/Ieee1609Dot2Data", true));
            protocol_.push_back(std::make_tuple(ASDFRAME, decode_asdframe_type, "AdvisorySituationData", false));

            break;
        case ASDFRAME_J2735MESSAGEFRAME:
            protocol_.push_back(std::make_tuple(J2735MESSAGEFRAME, decode_messageframe_type, "AdvisorySituationData/asdmDetails/advisoryMessage/MessageFrame", true));
            protocol_.push_back(std::make_tuple(ASDFRAME, decode_asdframe_type, "AdvisorySituationData", false));

            break;
        case ASDFRAME_IEEE1609DOT2_J2735MESSAGEFRAME:
            protocol_.push_back(std::make_tuple(J2735MESSAGEFRAME, decode_messageframe_type, "AdvisorySituationData/asdmDetails/advisoryMessage/Ieee1609Dot2Data/content/unsecuredData/MessageFrame", true));
            protocol_.push_back(std::make_tuple(IEEE1609DOT2, decode_1609dot2_type, "AdvisorySituationData/asdmDetails/advisoryMessage/Ieee1609Dot2Data", true));
            protocol_.push_back(std::make_tuple(ASDFRAME, decode_asdframe_type, "AdvisorySituationData", false));


            break;
        default:
            throw MissingInputElementError{"An encoder was not specified in the encodingType tag that this module understands."};

    }
    
    encode_for_protocol();
    
    // convert DOM to a RAW string representation: no spaces, no tabs.
    // for testing.
    input_doc.save(output_message_stream, "", pugi::format_raw);

    return true;
}

/** 
 * Decodes the IEEE 1609.2 ASN.1 bytes represented by the hex string according to the instance type variable:
 * decode_1609dot2_type into its C structure, then encodes the C structure into XML. The XML is put into the xml_buffer.
 *
 * This method does not NORMALLY modify the input_doc directly.
 * This method will modify the input_doc on error. 
 *
 * Return true on success: use the xml_buffer to generate valid XML to use to extract out the next layer.
 * Return false on failure: immediately use the input_doc to return what happened during decoding of 1609.2
 */

// throws Asn1CodecError ONLY!
bool ASN1_Codec::decode_1609dot2_data( std::string& data_as_hex, buffer_structure_t* xml_buffer ) {
    static const char* fnname = "decode_1609dot2_data()";

    // enum asn_dec_rval_code_e {
    // 	RC_OK,		                                  // successful decoding.
    // 	RC_WMORE,	                                  // more data expected.
    // 	RC_FAIL		                                  // failure to decode data.
    // };
    //
    // typedef struct asn_dec_rval_s {
    // 	enum asn_dec_rval_code_e code;                // one of the above codes.
    // 	size_t consumed;		                      // number of bytes consumed.
    // } asn_dec_rval_t;
    asn_dec_rval_t decode_rval;

    // typedef struct asn_enc_rval_s {
    // 	ssize_t encoded;                              // bytes encoded on success; -1 on fail
    // 	struct asn_TYPE_descriptor_s *failed_type;    // the type that failed.
    // 	      ->name  
    // 	void *structure_ptr;                          // pointer to structure of that type.
    // } asn_enc_rval_t;
    asn_enc_rval_t encode_rval;

    errlen = max_errbuf_size;

    Ieee1609Dot2Data_t *ieee1609data = 0;        // must initialize to 0 according to asn.1 instructions.

    ilogger->trace("{}: starting...", fnname);

    // remove all spaces.
    data_as_hex.erase( remove_if ( data_as_hex.begin(), data_as_hex.end(), isspace), data_as_hex.end());

    if (data_as_hex.empty()) {
        throw Asn1CodecError{"failed attempt to decode IEEE 1609.2 hex string: string empty."};
    }

    ilogger->trace("{}: success extracting {} hex string: {}", fnname , asn_DEF_Ieee1609Dot2Data.name, data_as_hex );

    byte_buffer.clear();
    if (!hex_to_bytes_(data_as_hex, byte_buffer)) {
        throw Asn1CodecError{"failed attempt to decode IEEE 1609.2 hex string: cannot convert to bytes."};
    }

    ilogger->trace("{}: successful conversion to raw byte buffer.", fnname );

    // Decode BAH Bytes (A 1609.2 Frame) into the appropriate structure.
    decode_rval = asn_decode( 
            0, 
            decode_1609dot2_type, 
            &asn_DEF_Ieee1609Dot2Data, 
            (void **)&ieee1609data, 
            byte_buffer.data(), 
            byte_buffer.size() 
            );

    if ( decode_rval.code != RC_OK ) {
        erroross.str("");
        erroross << "failed ASN.1 binary decoding of element " << asn_DEF_Ieee1609Dot2Data.name << ": ";
        if ( decode_rval.code == RC_FAIL ) {
            erroross << "bad data.";
        } else {
            erroross << "more data expected.";
        }
        erroross << " Successfully decoded " << decode_rval.consumed << " bytes.";
        throw Asn1CodecError{ erroross.str() };
    }

    ilogger->trace("{}: ASN.1 binary decode success.", fnname );

    // check the data in the returned structure against the ASN.1 specification constraints.
    if (asn_check_constraints( &asn_DEF_Ieee1609Dot2Data, ieee1609data, errbuf, &errlen )) {
        erroross.str("");
        erroross << "failed ASN.1 constraints check of element " << asn_DEF_Ieee1609Dot2Data.name << ": ";
        erroross.write( errbuf, errlen );
        ASN_STRUCT_FREE(asn_DEF_Ieee1609Dot2Data, ieee1609data);
        throw Asn1CodecError{ erroross.str() };
    }

    // target form is always XML (for now).
    encode_rval = xer_encode( 
            &asn_DEF_Ieee1609Dot2Data, 
            ieee1609data, 
            XER_F_CANONICAL, 
            dynamic_buffer_append, 
            static_cast<void *>(xml_buffer) 
            );

    ASN_STRUCT_FREE(asn_DEF_Ieee1609Dot2Data, ieee1609data);

    if ( encode_rval.encoded == -1 ) {
        erroross.str("");
        erroross << "failed ASN.1 XML encoding of Ieee1609Dot2Data element " << encode_rval.failed_type->name;
        throw Asn1CodecError{ erroross.str() };
    }

    ilogger->trace("{}: finished.", fnname );
    return true;
}

/**
 * TODO: This method should be generalizable to any type def and structure pointer -- tried but moved on.
 */
bool ASN1_Codec::decode_messageframe_data( std::string& data_as_hex, buffer_structure_t* xml_buffer ) {
    static const char* fnname = "decode_messageframe_data()";

    asn_dec_rval_t decode_rval;
    asn_enc_rval_t encode_rval;

    errlen = max_errbuf_size;

    MessageFrame_t *messageframe = 0;           // must be initialized to 0.

    ilogger->trace("{}: starting...", fnname);

    // remove all spaces.
    data_as_hex.erase( remove_if ( data_as_hex.begin(), data_as_hex.end(), isspace), data_as_hex.end());

    if (data_as_hex.empty()) {
        throw Asn1CodecError{"failed attempt to decode MessageFrame hex string: string empty."};
    }

    ilogger->trace("{}: success extracting {} hex string: {}", fnname , asn_DEF_MessageFrame.name, data_as_hex );

    byte_buffer.clear();
    if (!hex_to_bytes_(data_as_hex, byte_buffer)) {
        throw Asn1CodecError{"failed attempt to decode MessageFrame hex string: cannot convert to bytes."};
    }

    ilogger->trace("{}: successful conversion to raw byte buffer.", fnname );

    decode_rval = asn_decode( 
            0, 
            decode_messageframe_type, 
            &asn_DEF_MessageFrame,
            (void **)&messageframe,
            byte_buffer.data(), 
            byte_buffer.size() 
            );

    if ( decode_rval.code != RC_OK ) {
        erroross.str("");
        erroross << "failed ASN.1 binary decoding of element " << asn_DEF_MessageFrame.name << ": ";
        if ( decode_rval.code == RC_FAIL ) {
            erroross << "bad data.";
        } else {
            erroross << "more data expected.";
        }
        erroross << " Successfully decoded " << decode_rval.consumed << " bytes.";
        throw Asn1CodecError{ erroross.str() };
    }

    ilogger->trace("{}: ASN.1 binary decode successful.", fnname );

    if (asn_check_constraints( &asn_DEF_MessageFrame, messageframe, errbuf, &errlen )) {
        erroross.str("");
        erroross << "failed ASN.1 constraints check of element " << asn_DEF_MessageFrame.name << ": ";
        erroross.write( errbuf, errlen );
        ASN_STRUCT_FREE(asn_DEF_MessageFrame, messageframe);
        throw Asn1CodecError{ erroross.str() };
    }

    // Encode the Ieee1609Dot2Data ASN.1 C struct into XML, so we can extract out the BSM.
    encode_rval = xer_encode( 
            &asn_DEF_MessageFrame, 
            messageframe, 
            XER_F_CANONICAL, 
            dynamic_buffer_append, 
            static_cast<void *>(xml_buffer) 
            );

    ASN_STRUCT_FREE(asn_DEF_MessageFrame, messageframe);

    if ( encode_rval.encoded == -1 ) {
        erroross.str("");
        erroross << "failed ASN.1 XML encoding of MessageFrame element " << encode_rval.failed_type->name;
        throw Asn1CodecError{ erroross.str() };
    }

    ilogger->trace("{}: finished.", fnname );
    return true;
}
        
void ASN1_Codec::encode_frame_data(const std::string& data_as_xml, std::string& hex_string) {
    static const char* fnname = "encode_frame_data()";

    asn_dec_rval_t decode_rval;
    asn_enc_rval_t encode_rval;

	// TODO: working toward a general solution for these function; first is passing in a ref to 
	// these types of structures.
	struct asn_TYPE_descriptor_s* data_struct;
    void *frame_data = 0;

    switch (curr_op_) {
        case J2735MESSAGEFRAME:
            data_struct = &asn_DEF_MessageFrame;

            break;
        case IEEE1609DOT2:
            data_struct = &asn_DEF_Ieee1609Dot2Data;

            break;
        case ASDFRAME:
            data_struct = &asn_DEF_AdvisorySituationData;

            break;
        default:
            // TODO internal err
            break;
    }

    errlen = max_errbuf_size;

    decode_rval = xer_decode( 
            0 				// new parameter addition seems to work with nullptr.
			, data_struct
            , (void **)&frame_data
            , data_as_xml.data()
            , data_as_xml.size()
            );

    if ( decode_rval.code != RC_OK ) {
        erroross.str("");
        erroross << "failed ASN.1 decoding of XML element " << data_struct->name << ": ";
        if ( decode_rval.code == RC_FAIL ) {
            erroross << "bad data.";
        } else {
            erroross << "more data expected.";
        }
        erroross << " Successfully decoded " << decode_rval.consumed << " bytes.";
        throw Asn1CodecError{ erroross.str() };
    }

    if (asn_check_constraints( data_struct, frame_data, errbuf, &errlen )) {
        erroross.str("");
        erroross << "failed ASN.1 constraints check of element " << data_struct->name << ": ";
        erroross.write( errbuf, errlen );
        ASN_STRUCT_FREE(*data_struct, frame_data);
        throw Asn1CodecError{ erroross.str() };
    }

    buffer_structure_t buffer = {0,0,0};

    encode_rval = asn_encode(
        0,
        curr_decode_type_,
        data_struct,
        frame_data, 
        dynamic_buffer_append, 
        static_cast<void *>(&buffer) 
        );

    ASN_STRUCT_FREE(*data_struct, frame_data);

    if ( encode_rval.encoded == -1 ) {
        erroross.str("");
        erroross << "failed ASN.1 encoding of SDWTIM element " << encode_rval.failed_type->name;
        throw Asn1CodecError{ erroross.str() };
    }

    if (!bytes_to_hex_(&buffer, hex_string)) {
        std::free( static_cast<void *>(buffer.buffer) );
        throw Asn1CodecError{ "failed attempt to encode SDWTIM byte buffer into hex string." };
    }

    std::free( static_cast<void *>(buffer.buffer) );
}

bool ASN1_Codec::set_codec_requirements( pugi::xml_document& doc ) {

    static const char* fnname = "set_codec_requirements()";

    enum asn_transfer_syntax atstype = ATS_INVALID;
	opsflag = 0;

    // re-establish defaults.
    decode_1609dot2 = false;
    decode_messageframe = false;
    decode_asdframe = false;
    decode_1609dot2_type = ATS_CANONICAL_OER;
    decode_messageframe_type = ATS_UNALIGNED_BASIC_PER;


    // Determine which decodings are needed.
    // TODO: Think aobut using a xpath_nodeset structure and iterating.
    pugi::xpath_node encodings_xpath_node = ode_encodings_query.evaluate_node( input_doc );
    if (!encodings_xpath_node) {
        throw UnparseableInputError{"Failed to find path: OdeAsn1Data/metadata/encodings in the input file."};
    }

    for ( pugi::xml_node n = encodings_xpath_node.node().first_child(); n; n = n.next_sibling()) {

        pugi::xml_text ats_node = n.child("encodingRule").text();
        if ( ats_node ) {
            // the XML file contains the rule specification and we should use it.
            atstype = get_ats_transfer_syntax( ats_node.get() );
        }

		if ( atstype == ATS_INVALID ) {
			throw UnparseableInputError{"Invalid encoding rule in input file."};
		}
        
        // TODO: These strings ( must be detected as hard coded string or config parameters ).

        if ( std::strcmp(n.child("elementType").text().get(), "Ieee1609Dot2Data") == 0 ) {
			opsflag |= static_cast<uint32_t>(Asn1OpsType::IEEE1609DOT2);
            decode_1609dot2 = true;
            decode_1609dot2_type = atstype;

        } else if ( std::strcmp(n.child("elementType").text().get(), "MessageFrame") == 0 ) {
			opsflag |= static_cast<uint32_t>(Asn1OpsType::J2735MESSAGEFRAME);
            decode_messageframe = true;
            decode_messageframe_type = atstype;

        } else if ( std::strcmp(n.child("elementType").text().get(), "AdvisorySituationData") == 0 ) {
			opsflag |= static_cast<uint32_t>(Asn1OpsType::ASDFRAME);
            decode_asdframe = true;
            decode_asdframe_type = atstype;
        }
    }

    if (!opsflag) {
        throw UnparseableInputError{"Input file did not specify any encoding/decoding operations."};
    }

    return true;
}

bool ASN1_Codec::file_test(std::string file_path, std::ostream& os, bool encode) {
    static const char* fnname = "file_test()";

    std::stringstream output_msg_stream;
    bool r = true;

    std::FILE* ifile = std::fopen( file_path.c_str(), "r" );

    if (!ifile) {
        std::cerr << "cannot open " << file_path << '\n';
        return EXIT_FAILURE;
    }

    decode_functionality = !encode;

    // compute file size in bytes.
    std::fseek(ifile, 0, SEEK_END);
    std::size_t ifile_size = std::ftell(ifile);
    std::fseek(ifile, 0, SEEK_SET);

    std::vector<unsigned char> consumed_xml_buffer( ifile_size );
    std::size_t count = std::fread( consumed_xml_buffer.data(), sizeof( uint8_t ), consumed_xml_buffer.size(), ifile );
    std::fclose( ifile );

    if ( consumed_xml_buffer.size() > 0 ) {
        msg_recv_count++;
        msg_recv_bytes += consumed_xml_buffer.size();

        try {

            // pugi resets the document as part of load_buffer
            pugi::xml_parse_result result = input_doc.load_buffer((const void*) consumed_xml_buffer.data(), consumed_xml_buffer.size(), xml_parse_options );

            if (!result) {
                erroross.str("");
                erroross << "Input file parse error: " << result.description() << " at offset " << result.offset;
                throw UnparseableInputError{ erroross.str() };
            } 

            // examine the input xml encodings information and set the flags and requirements needed to properly parse the byte strings.
            set_codec_requirements( input_doc );            // throws.

            // Retain this node reference. It is where the decoded result will be inserted.

            payload_node_ = ode_payload_query.evaluate_node( input_doc ).node();
            if ( !payload_node_ ) {
                throw UnparseableInputError{ "Failed to find path: OdeAsn1Data/payload/data in the input document." };
            } 

            if ( decode_functionality ) {
                decode_message( payload_node_, output_msg_stream );
            } else {
                encode_message( output_msg_stream );
            }

        } catch (const UnparseableInputError& e) {

            r = false;
            //elogger->trace("{}: UnparseableInputError {}", fnname , e.what() );
            add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const MissingInputElementError& e) {

            r = false;
            //elogger->trace("{}: MissingInputElementError {}", fnname , e.what() );
            add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const pugi::xpath_exception& e ) {

            r = false;
            //elogger->trace("{}: pugi::xpath_exception {}", fnname, e.what() );
            add_error_xml( error_doc, Asn1DataType::ODE, Asn1ErrorType::REQUEST, e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const Asn1CodecError& e) {

            r = false;
            //elogger->trace("{}: Asn1CodecError {}", fnname , e.what() );
            add_error_xml( input_doc, e.data_type(), e.error_type(), e.what(), false );
            input_doc.save(output_msg_stream,"",pugi::format_raw);
        }

        os << output_msg_stream.str() << std::endl;
    }

    return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

/**
 * Used as a test stub to bypass Kafka and work through the parsing, encoding, decoding.
 *
 */
bool ASN1_Codec::filetest() {
    static const char* fnname = "filetest()";
    bool r = true;

    std::string error_string;
    RdKafka::ErrorCode status;
    std::stringstream output_msg_stream;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    try {

        if ( !configure() ) return EXIT_FAILURE;

    } catch ( std::exception& e ) {

        std::cerr << "Fatal Exception: " << e.what() << '\n';           // logger may have failed to configure.
        return EXIT_FAILURE;
    }

    ilogger->trace("{}: Starting...", fnname);

    std::FILE* ifile = std::fopen( operands[0].c_str(), "r" );
    if (!ifile) {
        std::cerr << "cannot open " << operands[0] << '\n';
        return EXIT_FAILURE;
    }

    // compute file size in bytes.
    std::fseek(ifile, 0, SEEK_END);
    std::size_t ifile_size = std::ftell(ifile);
    std::fseek(ifile, 0, SEEK_SET);

    std::vector<unsigned char> consumed_xml_buffer( ifile_size );
    std::size_t count = std::fread( consumed_xml_buffer.data(), sizeof( uint8_t ), consumed_xml_buffer.size(), ifile );
    std::fclose( ifile );

    if ( consumed_xml_buffer.size() > 0 ) {

        ilogger->trace("{}: successful read of the test file having {} bytes.", fnname, consumed_xml_buffer.size() );

        msg_recv_count++;
        msg_recv_bytes += consumed_xml_buffer.size();

        try {

            // pugi resets the document as part of load_buffer
            pugi::xml_parse_result result = input_doc.load_buffer((const void*) consumed_xml_buffer.data(), consumed_xml_buffer.size(), xml_parse_options );

            if (!result) {
                erroross.str("");
                erroross << "Input file parse error: " << result.description() << " at offset " << result.offset;
                throw UnparseableInputError{ erroross.str() };
            } 

            // examine the input xml encodings information and set the flags and requirements needed to properly parse the byte strings.
            set_codec_requirements( input_doc );            // throws.

            // Retain this node reference. It is where the decoded result will be inserted.
            payload_node_ = ode_payload_query.evaluate_node( input_doc ).node();

            if ( !payload_node_ ) {
                throw UnparseableInputError{ "Failed to find path: OdeAsn1Data/payload/data in the input document." };
            } 

            if ( decode_functionality ) {
                decode_message( payload_node_, output_msg_stream );
            } else {
                encode_message( output_msg_stream );
            }

        } catch (const UnparseableInputError& e) {

            r = false;
            elogger->trace("{}: UnparseableInputError {}", fnname , e.what() );
            add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const MissingInputElementError& e) {

            r = false;
            elogger->trace("{}: MissingInputElementError {}", fnname , e.what() );
            add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const pugi::xpath_exception& e ) {

            r = false;
            elogger->trace("{}: pugi::xpath_exception {}", fnname, e.what() );
            add_error_xml( error_doc, Asn1DataType::ODE, Asn1ErrorType::REQUEST, e.what(), true );
            error_doc.save(output_msg_stream,"",pugi::format_raw);

        } catch (const Asn1CodecError& e) {

            r = false;
            elogger->trace("{}: Asn1CodecError {}", fnname , e.what() );
            add_error_xml( input_doc, e.data_type(), e.error_type(), e.what(), false );
            input_doc.save(output_msg_stream,"",pugi::format_raw);

        }

        std::cout << output_msg_stream.str() << '\n';

    } else {
        ilogger->trace("Read an empty file.");
    }

    // NOTE: good for troubleshooting, but bad for performance.
    ilogger->trace("{}: Finished.", fnname);
    elogger->flush();
    ilogger->flush();

    return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

int ASN1_Codec::operator()(void) {

    static const char* fnname = "run()";

    RdKafka::ErrorCode status;
    std::string error_string;
    bool success = false;

    std::stringstream output_msg_stream;

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

    while (bootstrap) {
        // reset flag here, or else nothing works below
        data_available = true;

        if ( !launch_consumer() ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );
        
            continue;
        }

        if ( !launch_producer() ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

            continue;
        }

        // consume-produce loop.
        while (data_available) {

            std::unique_ptr<RdKafka::Message> msg{ consumer_ptr->consume( consumer_timeout ) };

            try {

                success = process_message( msg.get(), output_msg_stream );          // throws.

            } catch (const UnparseableInputError& e) {

                elogger->trace("{}: UnparseableInputError {}", fnname , e.what() );
                add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
                error_doc.save(output_msg_stream,"",pugi::format_raw);

            } catch (const MissingInputElementError& e) {

                elogger->trace("{}: MissingInputElementError {}", fnname , e.what() );
                add_error_xml( error_doc, e.data_type(), e.error_type(), e.what(), true );
                error_doc.save(output_msg_stream,"",pugi::format_raw);

            } catch (const pugi::xpath_exception& e ) {

                elogger->trace("{}: pugi::xpath_exception {}", fnname, e.what() );
                add_error_xml( error_doc, Asn1DataType::ODE, Asn1ErrorType::REQUEST, e.what(), true );
                error_doc.save(output_msg_stream,"",pugi::format_raw);

            } catch (const Asn1CodecError& e) {

                elogger->trace("{}: Asn1CodecError {}", fnname , e.what() );
                add_error_xml( input_doc, e.data_type(), e.error_type(), e.what(), false );
                input_doc.save(output_msg_stream,"",pugi::format_raw);

            }

            if ( msg->len() > 0 ) {

                std::cerr << msg->len() << " bytes consumed from topic: " << consumed_topics[0] << '\n';

                std::string output_msg_string = output_msg_stream.str();
                status = producer_ptr->produce(published_topic_ptr.get(), partition, RdKafka::Producer::RK_MSG_COPY, (void *)output_msg_string.c_str(), output_msg_string.size(), NULL, NULL);

                if (status != RdKafka::ERR_NO_ERROR) {
                    elogger->error("{}: Failure of XER encoding: {}", fnname , RdKafka::err2str( status ));

                } else {
                    // successfully sent; update counters.
                    msg_send_count++;
                    msg_send_bytes += output_msg_string.size();
                    ilogger->trace("{}: successful encoding/decoding", fnname );
                    std::cerr << output_msg_string.size() << " bytes produced to topic: " << published_topic_ptr->name() << '\n';
                }

                // clear out the stream
                output_msg_stream.str("");
                output_msg_stream.clear();
            } 

            // NOTE: good for troubleshooting, but bad for performance.
            elogger->flush();
            ilogger->flush();
        }
    }

    ilogger->info("{}: shutting down...", fnname );
    ilogger->info("ASN1_Codec consumed  : {} blocks and {} bytes", msg_recv_count, msg_recv_bytes);
    ilogger->info("ASN1_Codec published : {} blocks and {} bytes", msg_send_count, msg_send_bytes);

    std::cerr << "ASN1_Codec operations complete; shutting down...\n";
    std::cerr << "ASN1_Codec consumed   : " << msg_recv_count << " blocks and " << msg_recv_bytes << " bytes\n";
    std::cerr << "ASN1_Codec published  : " << msg_send_count << " blocks and " << msg_send_bytes << " bytes\n";
    return EXIT_SUCCESS;
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
    asn1_codec.addOption( 'T', "codec-type", "The type of codec to use: decode or encode; defaults to decode", true );


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
        // Only used when an input file is specified.
        std::exit( asn1_codec.filetest() );

    } else {
        // The module will run and when it terminates return an appropriate error code.
        std::exit( asn1_codec.run() );
    }
}

#endif
