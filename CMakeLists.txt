# cmake_minimum_required(VERSION 2.6)
cmake_minimum_required(VERSION 3.1)

project(ACM)

# need to set these prior to setting any targets.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_C_FLAGS "-O3")

if (${APPLE})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(MACPORTS_DIR "/opt")
endif ()

if(CMAKE_COMPILER_IS_GNUCXX)                                           # add coverage compiler option
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
endif()

if (${APPLE})
    include_directories( "${MACPORTS_DIR}/local/include")
    link_directories("${MACPORTS_DIR}/local/lib" "/usr/lib" "/usr/local/lib")
endif ()

#### BUILD TARGET FOR THE ASN1_Codec ####

# before the add_executable
link_directories("${CMAKE_SOURCE_DIR}/asn1c_combined")

# NOTE: This may need to be changed depending on install location.
link_directories("/usr/local/lib")

# CPP Preprocessor variable PDU is used in the asn1-j2735-lib.h file to build a variable to a ASN.1 parsing structure.
# This variable may need changing if we are parsing some other type of ASN.1 schema.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPDU=MessageFrame")

# Use the include + target_sources pattern; this just sets up the container for the list of source files.
add_executable(acm "")
# add_executable(acm-blob-producer "")

include( "src/CMakeLists.txt" )

target_link_libraries(acm pthread rdkafka++ asncodec pugixml)

# Copy the data to the build. TODO make this part of the test or data target.
set(DATA_DIR $<TARGET_FILE_DIR:acm>/data)
set(CONFIG_DIR $<TARGET_FILE_DIR:acm>/config)

# use cmake command mode (-E) commands to make and move files to build directory

# Make the base data directory.
add_custom_command(TARGET acm POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${DATA_DIR})

# Copy the data files.
add_custom_command(TARGET acm POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${DATA_DIR})

# Make the base data directory.
add_custom_command(TARGET acm POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CONFIG_DIR})

# Copy the data files.
add_custom_command(TARGET acm POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config ${CONFIG_DIR})

# option(BUILD_TESTS "Determines whether to build tests." ON)
# if(BUILD_TESTS)
#     enable_testing()
#     add_test(NAME mytest1 COMMAND ASN1_Codec_tests)
# endif()
