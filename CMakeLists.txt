cmake_minimum_required(VERSION 3.0.2)

project(libdocument VERSION 0.0.1 LANGUAGES C CXX)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Sources of LibDocument:
file(GLOB_RECURSE libdocument_SRC
    FOLLOW_SYMLINKS
    "include/*.h"
    "src/lib/*.c")

# Tests -- unit tests, integration tests (add acceptance tests if you are eager):
file(GLOB_RECURSE test_SRC
    FOLLOW_SYMLINKS
    "test/*.h"
    "test/*.cpp")

# Create 'test' target in Makefiles:
enable_testing()

#
# External dependencies:
#

include(ExternalProject)

ExternalProject_Add(libbson-lib
    GIT_REPOSITORY https://github.com/mongodb/libbson.git
    GIT_TAG 1.1.0
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson
    CONFIGURE_COMMAND /usr/bin/env bash -l ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson/autogen.sh
    BUILD_COMMAND ${MAKE})

# Dependency googletest -- note that SOURCE_DIR and BINARY_DIR are the same:
ExternalProject_Add(googletest-lib
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk
    SVN_REVISION -r "700"
    SVN_TRUST_CERT 1
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/googletest
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/googletest
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND autoreconf -fi COMMAND ./configure
    BUILD_COMMAND ${MAKE}
    INSTALL_COMMAND true)

# Dependency Doxygen:
ExternalProject_Add(doxygen
    GIT_REPOSITORY https://github.com/doxygen/doxygen.git
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/doxygen
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/doxygen
    CONFIGURE_COMMAND ./configure
    BUILD_COMMAND ${MAKE})

#
# libdocument generation:
#

include_directories(include)
include_directories(src/googletest/include)

link_directories(lib)

add_library(document SHARED ${libdocument_SRC})

add_library(document-static STATIC ${libdocument_SRC})
set_target_properties(document-static PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY lib
    LIBRARY_OUTPUT_DIRECTORY_DEBUG lib
    LIBRARY_OUTPUT_DIRECTORY_RELEASE lib
    IMPORT_LOCATION lib/${CMAKE_CFG_INTDIR}/libdocument${CMAKE_STATIC_LIBRARY_SUFFIX})

# Make sure headers are visible to the example executable below:
include_directories(src/lib src/libbson/src/bson libbson-lib-prefix/src/libbson-lib-build/src/bson)

# Executables:
add_executable(libdocument-ex1 src/examples/ex1.c)
add_dependencies(libdocument-ex1 libbson-lib)

add_executable(tests ${test_SRC})
add_dependencies(tests document-static googletest-lib)

#
# Linking information for executables:
#

target_link_libraries(libdocument-ex1 document)

# As suggested in the googletest README: link libgtest statically:
add_library(gtest STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION src/googletest/lib/.libs/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX})
target_link_libraries(tests document-static gtest)
