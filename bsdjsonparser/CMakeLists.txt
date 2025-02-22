cmake_minimum_required(VERSION 3.15)
project(bsdjsonparser LANGUAGES CXX)

add_library(bsdjsonparser
			src/BSDJSONParserBase.cpp
			src/BSDAverageUserAge.cpp
			src/BSDAverageFriendCount.cpp
			src/BSDMostCommonFirstName.cpp
			src/BSDMostCommonHobby.cpp
			src/BSDUserWithBiggestFriendCount.cpp
			)
include_directories(include)

target_include_directories(bsdjsonparser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS bsdjsonparser
    EXPORT my_library-export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

#install(EXPORT bsdjsonparser-export
#    DESTINATION lib/cmake/bsdjsonparser
#    FILE bsdjsonparser-config.cmake
#)

#export(PACKAGE bsdjsonparser)

#package(
#    FILES "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/bsdjsonparser/bsdjsonparser-config.cmake"
#    DESTINATION lib/cmake/bsdjsonparser
#)
