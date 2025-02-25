cmake_minimum_required(VERSION 3.10)

#use -DCMAKE_BUILD_TYPE=Debug instead
#set(CMAKE_BUILD_TYPE Debug)

#add_subdirectory(../bsdjsonparser)
#include_directories(../bsdjsonparser/include)

# Set some basic project attributes
project (local_json_file_tester
	VERSION 0.1
	DESCRIPTION "local file json parsing tester")

# This project will output an executable file
add_executable(${PROJECT_NAME} local_json_file_tester.cpp ../RestAPIHandler.cpp)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr bsdjsonparser)
