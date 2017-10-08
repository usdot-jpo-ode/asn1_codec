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

#include <librdkafka/rdkafkacpp.h>
#include "tool.hpp"
#include "spdlog/spdlog.h"

class ACMBlobProducer : public tool::Tool {

    public:

        std::shared_ptr<spdlog::logger> ilogger;
        std::shared_ptr<spdlog::logger> elogger;

        static void sigterm (int sig);

        ACMBlobProducer( const std::string& name, const std::string& description );
        ~ACMBlobProducer();

        void print_configuration() const;
        bool configure();
        bool launch_producer();
        int operator()(void);

        /**
         * @brief Create and setup the two loggers used for the ACMBlobProducer. The locations and filenames for the logs can be specified
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

        static bool data_available;                                     ///> flag to exit application; set via signals so static.

        static constexpr long ilogsize = 1048576 * 5;                   ///> The size of a single information log; these rotate.
        static constexpr long elogsize = 1048576 * 2;                   ///> The size of a single error log; these rotate.
        
        // If this buffer size is too small then an entire record will not be processed and things will FAIL!
        // buffer for the ASN1 stuff.
        static constexpr std::size_t BUFSIZE = 1<<12;                   ///> 4k
        uint8_t buf[BUFSIZE];

        static constexpr int ilognum = 5;                               ///> The number of information logs to rotate.
        static constexpr int elognum = 2;                               ///> The number of error logs to rotate.

        // counters.
        uint64_t msg_send_count;                                            ///> Counter for the number of BSMs published.
        uint64_t msg_send_bytes;                                         ///> Counter for the nubmer of BSM bytes published.

        spdlog::level::level_enum iloglevel;                            ///> Log level for the information log.
        spdlog::level::level_enum eloglevel;                            ///> Log level for the error log.
        std::string debug;
        std::string input_file;
        std::size_t block_size;

        int32_t partition;
        std::string published_topic_name;                                    ///> The topic we are publishing filtered BSM to.

        // configurations; global and topic (the names in these are fixed)
        std::unordered_map<std::string, std::string> mconf;
        RdKafka::Conf *conf;
        RdKafka::Conf *tconf;

        std::shared_ptr<RdKafka::Producer> producer_ptr;
        std::shared_ptr<RdKafka::Topic> published_topic_ptr;
};

