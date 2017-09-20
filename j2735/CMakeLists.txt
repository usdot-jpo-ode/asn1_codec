cmake_minimum_required(VERSION 3.1)

set(CMAKE_VERBOS_MAKEFILE on)

project(j2735dump LANGUAGES C CXX VERSION 0.1)

set(ASN1C "${CMAKE_CURRENT_SOURCE_DIR}/../asn1c/skeletons")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASN_CONVERTER_TITLE='J2735 DSRC decoder' -DASN_DISABLE_OER_SUPPORT -DPDU=MessageFrame")

set(CMAKE_CXX_STANDARD 11)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${ASN1C}")

file(GLOB j2735_SOURCES *.c)

add_executable(j2735dump ${j2735_SOURCES})



