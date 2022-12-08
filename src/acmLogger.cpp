#include "../include/acmLogger.hpp"

AcmLogger::AcmLogger(std::string ilogname, std::string elogname) {
    // pull in the file & console flags from the environment
    initializeFlagValuesFromEnvironment();
    
    // setup information logger.
    std::vector<spdlog::sink_ptr> infoSinks;
    if (logToFileFlag) {
        infoSinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(ilogname, INFO_LOG_SIZE, INFO_LOG_NUM));
    }
    if (logToConsoleFlag) {
        infoSinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
    }
    setInfoLogger(std::make_shared<spdlog::logger>("ilog", begin(infoSinks), end(infoSinks)));
    set_info_level( iloglevel );
    set_info_pattern("[%C%m%d %H:%M:%S.%f] [%l] %v");

    // setup error logger.
    std::vector<spdlog::sink_ptr> errorSinks;
    if (logToFileFlag) {
        errorSinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(elogname, ERROR_LOG_SIZE, ERROR_LOG_NUM));
    }
    if (logToConsoleFlag) {
        errorSinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
    }
    setErrorLogger(std::make_shared<spdlog::logger>("elog", begin(errorSinks), end(errorSinks)));
    set_error_level( eloglevel );
    set_error_pattern("[%C%m%d %H:%M:%S.%f] [%l] %v");
}

void AcmLogger::set_info_level(spdlog::level::level_enum level) {
    ilogger->set_level( level );
}

void AcmLogger::set_error_level(spdlog::level::level_enum level) {
    elogger->set_level( level );
}

void AcmLogger::set_info_pattern(const std::string& pattern) {
    ilogger->set_pattern( pattern );
}

void AcmLogger::set_error_pattern(const std::string& pattern) {
    elogger->set_pattern( pattern );
}

void AcmLogger::info(const std::string& message) {
    ilogger->info(message.c_str());
}

void AcmLogger::error(const std::string& message) {
    elogger->error(message.c_str());
}

void AcmLogger::trace(const std::string& message) {
    ilogger->trace(message.c_str());
}

void AcmLogger::critical(const std::string& message) {
    elogger->critical(message.c_str());
}

void AcmLogger::warn(const std::string& message) {
    elogger->warn(message.c_str());
}

void AcmLogger::flush() {
    ilogger->flush();
    elogger->flush();
}

void AcmLogger::setInfoLogger(std::shared_ptr<spdlog::logger> spdlog_logger) {
    ilogger = spdlog_logger;
}

void AcmLogger::setErrorLogger(std::shared_ptr<spdlog::logger> spdlog_logger) {
    elogger = spdlog_logger;
}

void AcmLogger::initializeFlagValuesFromEnvironment() {
    std::string logToFileFlagString = getEnvironmentVariable("ACM_LOG_TO_FILE");
    std::string logToConsoleFlagString = getEnvironmentVariable("ACM_LOG_TO_CONSOLE");
    logToFileFlag = convertStringToBool(logToFileFlagString);
    logToConsoleFlag = convertStringToBool(logToConsoleFlagString);

    if (!logToFileFlag && !logToConsoleFlag) {
        std::cout << "WARNING: ACM_LOG_TO_FILE and ACM_LOG_TO_CONSOLE are both set to false. No logging will occur." << std::endl;
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