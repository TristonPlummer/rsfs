# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.1)

# Set the project details
project(rsfs            VERSION 1.0
        DESCRIPTION     "RuneScape filesystem library"
        LANGUAGES       CXX)

# Set language standard
set(CMAKE_CXX_STANDARD 20)

# Set macro directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/macros")

# Must use GNUInstallDirs to install libraries into correct locations on all platforms.
include(GNUInstallDirs)

# Include the library dependencies
add_subdirectory(deps)

# Include the library source code
add_subdirectory(rsfs)