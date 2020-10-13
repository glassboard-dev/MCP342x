# Set min req version of Cmake
cmake_minimum_required(VERSION 3.10)

# Set project name
project(MCP342x_Example_Main)

# Set our lib include directories
include_directories(../inc)

# Create the C Executable
add_executable(mcp342x main.c)
# Link against our icm20948 lib
TARGET_LINK_LIBRARIES(mcp342x "${CMAKE_SOURCE_DIR}/../lib/libmcp342x.a")
