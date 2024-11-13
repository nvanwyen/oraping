# project
project(oraping)

# requires cmake version 2.8 or greater
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

set(ORAPING_SOURCE_DIR ${CMAKE_SOURCE_DIR})
 
#
set(CMAKE_MODULE_PATH ${ORAPING_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH})

set(CMAKE_MODULE_PATH ${ORAPING_SOURCE_DIR}/cmake/modules
    ${CMAKE_MODULE_PATH})

# version
set(ORAPING_VERSION "")
file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION"
    _ORAPING_VERSION REGEX "version:")
string(REGEX REPLACE "version: " "" ORAPING_VERSION ${_ORAPING_VERSION})

#
include(Utilities)

#
colormsg(HIBLUE "Configuring Ora*Ping version: ${ORAPING_VERSION}")

#
add_subdirectory(source)

