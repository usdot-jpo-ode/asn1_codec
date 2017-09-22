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

#include "acm_blob_producer.hpp"
#include "utilities.hpp"
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

bool ACMBlobProducer::data_available = true;

void ACMBlobProducer::sigterm (int sig) {
    data_available = false;
}

ACMBlobProducer::ACMBlobProducer( const std::string& name, const std::string& description ) :
    Tool{ name, description },
    msg_send_count{0},
    msg_send_bytes{0},
    iloglevel{ spdlog::level::trace },
    eloglevel{ spdlog::level::err },
    mconf{},
    partition{RdKafka::Topic::PARTITION_UA},
    debug{""},
    block_size{BUFSIZE},
    published_topic_name{},
    conf{nullptr},
    tconf{nullptr},
    producer_ptr{},
    published_topic_ptr{},
    ilogger{},
    elogger{}
{
}

ACMBlobProducer::~ACMBlobProducer() 
{
    //if (consumer_ptr) consumer_ptr->close();

    // free raw librdkafka pointers.
    if (tconf) delete tconf;
    if (conf) delete conf;

    // TODO: This librdkafka item seems wrong...
    RdKafka::wait_destroyed(5000);    // pause to let RdKafka reclaim resources.
}

void ACMBlobProducer::print_configuration() const
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

    std::cout << "# Module Specific config \n";
    for ( const auto& m : mconf ) {
        std::cout << m.first << " = " << m.second << '\n';
    }
}

/**
 * The following configuration settings are processed by configure:
 *
 asn1.j2735.kafka.partition
 asn1.j2735.topic.consumer
 asn1.j2735.topic.producer
 asn1.j2735.consumer.timeout.ms

JMC: TODO: All the ASN1C variables that are normally setup do here!

*/
bool ACMBlobProducer::configure() {

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
    if ( !optIsSet('F') ) {
        elogger->error( "Must specify the path to an input binary file." );
        return false;
    }

    input_file = optString('F');

    if ( !fileExists( input_file ) ) {
        elogger->error( "The input file: {} does not exist.", input_file );
        return false;
    }

    ilogger->info("using input file: {}", input_file );

    if ( optIsSet('B') ) {
        try {
            block_size = optInt('B');
        } catch ( std::exception& e ) {
            block_size = BUFSIZE;
        }
    }

    if ( !fileExists( input_file ) ) {
        elogger->error( "The input file: {} does not exist.", input_file );
        return false;
    }

    ilogger->info("using input file: {}", input_file );

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
                    ilogger->info("ACMBlobProducer configuration: {} = {}", pieces[0], pieces[1]);
                    // These configuration options are not expected by Kafka.
                    // Assume there are for the ACMBlobProducer.
                    mconf[ pieces[0] ] = pieces[1];
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
        auto search = mconf.find("asn1.j2735.kafka.partition");
        if ( search != mconf.end() ) {
            partition = std::stoi(search->second);              // throws.    
        }  // otherwise leave at default; PARTITION_UA
    }

    ilogger->info("kafka partition: {}", partition);

    if ( getOption('g').isSet() && conf->set("group.id", optString('g'), error_string) != RdKafka::Conf::CONF_OK) {
        // NOTE: there are some checks in librdkafka that require this to be present and set.
        elogger->error("kafka error setting configuration parameters group.id h: {}", error_string);
        return false;
    }

    if (optIsSet('d') && conf->set("debug", optString('d'), error_string) != RdKafka::Conf::CONF_OK) {
        elogger->error("kafka error setting configuration parameter debug: {}", error_string);
        return false;
    }

    // librdkafka defined configuration.
    conf->set("default_topic_conf", tconf, error_string);

    if (optIsSet('t')) {
        // this is the produced (filtered) topic.
        published_topic_name = optString( 't' );

    } else {
        // maybe it was specified in the configuration file.
        auto search = mconf.find("asn1.j2735.topic.producer");
        if ( search != mconf.end() ) {
            published_topic_name = search->second;
        } else {
            elogger->error("no publisher topic was specified; must fail.");
            return false;
        }
    }

    ilogger->info("published topic: {}", published_topic_name);
    ilogger->trace("ending configure()");
    return true;
}

/**
 *
 *
 */
bool ACMBlobProducer::launch_producer()
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

bool ACMBlobProducer::make_loggers( bool remove_files )
{
    // defaults.
    std::string path{ "logs/" };
    std::string ilogname{ "log.bproducer.info" };
    std::string elogname{ "log.bproducer.error" };

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

int ACMBlobProducer::operator()(void) {

    std::string error_string;
    RdKafka::ErrorCode status;
    FILE *source;
    std::size_t bytes_read = 0;
    int file_round = 0;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    
    try {

        // throws for mapfile and other items.
        if ( !configure() ) return EXIT_FAILURE;

    } catch ( std::exception& e ) {

        // don't use logger in case we cannot configure it correctly.
        std::cerr << "Fatal Exception: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if ( !launch_producer() ) return false;

    while (data_available) {
        // data_available changed via interupt SIGINT or SIGTERM

        // Process
        // 1. Read in the uper file.
        // 2. "Produce" some sized blocks of the file to the kafka topic with size indicated.

        source = fopen( input_file.c_str(), "rb" );

        if ( !source ) {
            elogger->error("No file: {}; cannot be opened for decoding.",input_file);
            return false;
        } 

        while ( !feof( source ) ) {

            // attempt to read 1K bytes into buffer.
            bytes_read = fread( buf, sizeof buf[0], block_size, source );

            if ( bytes_read > 0 ) {

                status = producer_ptr->produce(published_topic_ptr.get(), partition, RdKafka::Producer::RK_MSG_COPY, (void *)buf, bytes_read, NULL, NULL);

                if (status != RdKafka::ERR_NO_ERROR) {
                    elogger->error("Production failure code {} after reading {} bytes.", RdKafka::err2str( status ), bytes_read);
                    break;

                } else {
                    // successfully sent; update counters.
                    msg_send_count++;
                    msg_send_bytes += bytes_read;
                    ilogger->trace("Production success of {} bytes.", bytes_read);
                }


                std::cerr << "Bytes from file: " << bytes_read << ". Successfully produced to: " << published_topic_name << '\n';
            }
        }

        fclose( source );
        ilogger->info( "Finished producing the entire file.");
        std::cerr << "Sleeping for 5 seconds after file round " << ++file_round << "\n";
        std::this_thread::sleep_for( std::chrono::seconds(5) ); 
    }

    ilogger->info("ACMBlobProducer operations complete; shutting down...");
    ilogger->info("ACMBlobProducer published : {} blocks and {} bytes", msg_send_count, msg_send_bytes);
    std::cerr << "ACMBlobProducer published : " << msg_send_count << " binary blocks of size: " << block_size << " for " << msg_send_bytes << " bytes.\n";
    
    // NOTE: good for troubleshooting, but bad for performance.
    elogger->flush();
    ilogger->flush();
    return EXIT_SUCCESS;
}

#ifndef _ASN1_CODEC_TESTS

int main( int argc, char* argv[] )
{
    ACMBlobProducer acm_blob_producer{"ACMBlobProducer","ASN1 Processing Module"};

    acm_blob_producer.addOption( 'c', "config", "Configuration file name and path.", true );
    acm_blob_producer.addOption( 'C', "config-check", "Check the configuration file contents and output the settings.", false );
    acm_blob_producer.addOption( 't', "produce-topic", "The name of the topic to produce.", true );
    acm_blob_producer.addOption( 'p', "partition", "Consumer topic partition from which to read.", true );
    acm_blob_producer.addOption( 'g', "group", "Consumer group identifier", true );
    acm_blob_producer.addOption( 'b', "broker", "Broker address (localhost:9092)", true );
    acm_blob_producer.addOption( 'd', "debug", "debug level.", true );
    acm_blob_producer.addOption( 'v', "log-level", "The info log level [trace,debug,info,warning,error,critical,off]", true );
    acm_blob_producer.addOption( 'D', "log-dir", "Directory for the log files.", true );
    acm_blob_producer.addOption( 'R', "log-rm", "Remove specified/default log files if they exist.", false );
    acm_blob_producer.addOption( 'i', "ilog", "Information log file name.", true );
    acm_blob_producer.addOption( 'e', "elog", "Error log file name.", true );
    acm_blob_producer.addOption( 'F', "file", "Input binary file", true );
    acm_blob_producer.addOption( 'B', "blocksize", "The block size to read and write.", true );
    acm_blob_producer.addOption( 'h', "help", "print out some help" );

    if (!acm_blob_producer.parseArgs(argc, argv)) {
        acm_blob_producer.usage();
        std::exit( EXIT_FAILURE );
    }

    if (acm_blob_producer.optIsSet('h')) {
        acm_blob_producer.help();
        std::exit( EXIT_SUCCESS );
    }

    // can set levels if needed here.
    if ( !acm_blob_producer.make_loggers( acm_blob_producer.optIsSet('R') )) {
        std::exit( EXIT_FAILURE );
    }

    // configuration check.
    if (acm_blob_producer.optIsSet('C')) {
        try {
            if (acm_blob_producer.configure()) {
                acm_blob_producer.print_configuration();
                std::exit( EXIT_SUCCESS );
            } else {
                std::cerr << "Current configuration settings do not work.\n";
                acm_blob_producer.ilogger->error( "current configuration settings do not work; exiting." );
                std::exit( EXIT_FAILURE );
            }
        } catch ( std::exception& e ) {
            std::cerr << "Fatal Exception: " << e.what() << '\n';
            acm_blob_producer.elogger->error( "exception: {}", e.what() );
            std::exit( EXIT_FAILURE );
        }
    }

    // The module will run and when it terminates return an appropriate error code.
    std::exit( acm_blob_producer.run() );
}

#endif
