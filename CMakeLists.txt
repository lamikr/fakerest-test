cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(bsdjsonparser)
include_directories(bsdjsonparser/include)

# Set some basic project attributes
project (brightsign
	VERSION 0.1
	DESCRIPTION "A Hello World Project")
	
find_package(cpr REQUIRED)

# This project will output an executable file
add_executable(${PROJECT_NAME} brightsign.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr bsdjsonparser)
