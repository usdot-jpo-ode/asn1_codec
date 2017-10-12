# use the include command in the calling CMakeLists.txt file with this file as an argument.

# The sources in this directory that are needed for compilation.
target_sources(acm PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/acm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/utilities.cpp"
    )

# Include here all the relevant code for the above sources.
# Use this project's base source directory for these paths.
target_include_directories(acm PUBLIC
    "${ACM_SOURCE_DIR}/include"
    "${ACM_SOURCE_DIR}/include/catch"
    "${ACM_SOURCE_DIR}/include/rapidjson"
    "${ACM_SOURCE_DIR}/include/spdlog"
    "${ACM_SOURCE_DIR}/asn1c/skeletons"
    "${ACM_SOURCE_DIR}/asn1c_combined"
    "/usr/local/include"
    "/usr/local/include/librdkafka"
    )

# # The sources in this directory that are needed for compilation.
# target_sources(aem PUBLIC
#     "${CMAKE_CURRENT_LIST_DIR}/aem.cpp"
#     "${CMAKE_CURRENT_LIST_DIR}/tool.cpp"
#     "${CMAKE_CURRENT_LIST_DIR}/utilities.cpp"
#     )
# 
# # Include here all the relevant code for the above sources.
# # Use this project's base source directory for these paths.
# target_include_directories(aem PUBLIC
#     "${ACM_SOURCE_DIR}/include"
#     "${ACM_SOURCE_DIR}/include/catch"
#     "${ACM_SOURCE_DIR}/include/rapidjson"
#     "${ACM_SOURCE_DIR}/include/spdlog"
#     "${ACM_SOURCE_DIR}/asn1c/skeletons"
#     "${ACM_SOURCE_DIR}/asn1c_combined"
#     "/usr/local/include"
#     "/usr/local/include/librdkafka"
#     )

    #    "${acm_SOURCE_DIR}/j2735/include"
    #    "${acm_SOURCE_DIR}/asn1c_j2735"

    # # The sources in this directory that are needed for compilation.
    # target_sources(acm-blob-producer PUBLIC
    #     "${CMAKE_CURRENT_LIST_DIR}/acm_blob_producer.cpp"
    #     "${CMAKE_CURRENT_LIST_DIR}/tool.cpp"
    #     "${CMAKE_CURRENT_LIST_DIR}/utilities.cpp"
    #     )
    # 
    # # Include here all the relevant code for the above sources.
    # # Use this project's base source directory for these paths.
    # target_include_directories(acm-blob-producer PUBLIC
    #     "${acm_SOURCE_DIR}/include"
    #     "${acm_SOURCE_DIR}/include/catch"
    #     "${acm_SOURCE_DIR}/include/spdlog"
    #     )
    # 
    # target_sources(test_xml PUBLIC
    #     "${CMAKE_CURRENT_LIST_DIR}/test_xml.cpp"
    #     )
    # 
    # # Include here all the relevant code for the above sources.
    # # Use this project's base source directory for these paths.
    # target_include_directories(test_xml PUBLIC
    #     "${acm_SOURCE_DIR}/include"
    #     "${acm_SOURCE_DIR}/j2735/include"
    #     "${acm_SOURCE_DIR}/asn1c_j2735"
    #     "${acm_SOURCE_DIR}/asn1c/skeletons"
    #     )
