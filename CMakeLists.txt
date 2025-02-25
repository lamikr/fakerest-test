cmake_minimum_required(VERSION 3.10)

#use -DCMAKE_BUILD_TYPE=Debug instead
#set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(bsdjsonparser)
include_directories(bsdjsonparser/include)

# Set some basic project attributes
project (brightsign
	VERSION 0.1
	DESCRIPTION "JSON API Fech and Parsing Example")
	
find_package(cpr REQUIRED)

# This project will output an executable file
add_executable(${PROJECT_NAME} Brightsign.cpp RestAPIHandler.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr bsdjsonparser)

add_subdirectory(tests)

add_custom_target(
    doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs
    COMMAND cat README.md | md-to-pdf > README.pdf
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation with md-to-pdf"
    VERBATIM
)
