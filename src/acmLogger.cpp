#include "../include/acmLogger.hpp"

AcmLogger::AcmLogger(std::string logname) {
    // pull in the file & console flags from the environment
    initializeFlagValuesFromEnvironment();
    
    // setup spdlogger logger.
    std::vector<spdlog::sink_ptr> sinks;
    if (logToFileFlag) {
        std::cout << "Log to file flag set" << std::endl;
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logname, LOG_SIZE, LOG_NUM));
    }
    if (logToConsoleFlag) {
        std::cout << "Log to console flag set" << std::endl;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
    }
    setLogger(std::make_shared<spdlog::logger>("log", begin(sinks), end(sinks)));

    std::cout << "log level: " << loglevel << std::endl;
    set_level(loglevel);
    set_pattern("[%C%m%d %H:%M:%S.%f] [%l] %v");
}

void AcmLogger::set_level(spdlog::level::level_enum level) {
    spdlogger->set_level(level);
}

void AcmLogger::set_pattern(const std::string& pattern) {
    spdlogger->set_pattern(pattern);
}

void AcmLogger::info(const std::string& message) {
    spdlogger->info(message.c_str());
}

void AcmLogger::error(const std::string& message) {
    spdlogger->error(message.c_str());
}

void AcmLogger::trace(const std::string& message) {
    spdlogger->trace(message.c_str());
}

void AcmLogger::critical(const std::string& message) {
    spdlogger->critical(message.c_str());
}

void AcmLogger::warn(const std::string& message) {
    spdlogger->warn(message.c_str());
}

void AcmLogger::flush() {
    spdlogger->flush();
}

void AcmLogger::setLogger(std::shared_ptr<spdlog::logger> spdlog_logger) {
    spdlogger = spdlog_logger;
}

void AcmLogger::initializeFlagValuesFromEnvironment() {
    std::string logToFileFlagString = getEnvironmentVariable("ACM_LOG_TO_FILE");
    std::string logToConsoleFlagString = getEnvironmentVariable("ACM_LOG_TO_CONSOLE");
    logToFileFlag = convertStringToBool(logToFileFlagString);
    logToConsoleFlag = convertStringToBool(logToConsoleFlagString);

    if (!logToFileFlag && !logToConsoleFlag) {
        std::cout << "WARNING: The ACM_LOG_TO_FILE and ACM_LOG_TO_CONSOLE environment variables are both set to false. No logging will occur." << std::endl;
    }

    std::string logLevelString = getEnvironmentVariable("ACM_LOG_LEVEL");
    if ("TRACE" == logLevelString) {
        loglevel = spdlog::level::trace;
    } else if ("DEBUG" == logLevelString) {
        loglevel = spdlog::level::debug;
    } else if ("INFO" == logLevelString) {
        loglevel = spdlog::level::info;
    } else if ("WARNING" == logLevelString) {
        loglevel = spdlog::level::warn;
    } else if ("ERROR" == logLevelString) {
        loglevel = spdlog::level::err;
    } else if ("CRITICAL" == logLevelString) {
        loglevel = spdlog::level::critical;
    } else if ("OFF" == logLevelString) {
        loglevel = spdlog::level::off;
    } else {
        std::cout << "WARNING: information logger level was configured but unreadable; using default." << std::endl;
    }
}

const char* AcmLogger::getEnvironmentVariable(std::string variableName) {
    char* variableValue = getenv(variableName.c_str());
    if (variableValue == NULL) {
        return "";
    }
    return variableValue;
}

std::string AcmLogger::toLowercase(std::string s) {
    int counter = 0;
    char c;
    while (s[counter]) {
        c = s[counter];
        s[counter] = tolower(c);
        counter++;
    }
    return s;
}

bool AcmLogger::convertStringToBool(std::string value) {
    std::string lowercaseValue = toLowercase(value);
    if (lowercaseValue == "true" || lowercaseValue == "1") {
        return true;
    }
    return false;
}