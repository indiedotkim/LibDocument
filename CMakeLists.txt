cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_BUILD_TYPE Release)

project(libdocument VERSION 0.1.1 LANGUAGES C CXX)

#
# CUSTOMIZATION SECTION
#
# If you need to change CPython, googletest version numbers, then do this here!
#

set(PYTHON_VERSION "3.9")
set(PYTHON_PATCH "1")
set(PYTHON_SUFFIX "")

set(GOOGLETEST_VERSION "1.10")
set(GOOGLETEST_PATCH "0")

if(APPLE)
# Currently: Big Sur and beyond.
set(MACOSX_DEPLOYMENT_TARGET "11.0")
endif(APPLE)

#
# CUSTOMIZATION DONE
#

# Do not touch these:
set(CMAKE_CXX_STANDARD 17)
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

# Currently not used; might be added again in the future!?
# ExternalProject_Add(libbson-lib
#     GIT_REPOSITORY https://github.com/mongodb/libbson.git
#     GIT_TAG 1.1.0
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson
#     BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson
#     CONFIGURE_COMMAND /usr/bin/env bash -l ${CMAKE_CURRENT_SOURCE_DIR}/src/libbson/autogen.sh
#     BUILD_COMMAND ${MAKE})

ExternalProject_Add(python
    GIT_REPOSITORY https://github.com/python/cpython.git
    GIT_TAG v${PYTHON_VERSION}.${PYTHON_PATCH}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/python
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/python
    CONFIGURE_COMMAND MACOSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET} ./configure
    BUILD_COMMAND ${MAKE}
    INSTALL_COMMAND true)

# Dependency googletest -- note that SOURCE_DIR and BINARY_DIR are the same:
ExternalProject_Add(googletest-lib
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-${GOOGLETEST_VERSION}.${GOOGLETEST_PATCH}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/googletest
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/googletest
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND cmake -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles"
    BUILD_COMMAND ${MAKE}
    INSTALL_COMMAND true)

# Dependency Doxygen:
#ExternalProject_Add(doxygen
#    GIT_REPOSITORY https://github.com/doxygen/doxygen.git
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/doxygen
#    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/doxygen
#    CONFIGURE_COMMAND ./configure
#    BUILD_COMMAND ${MAKE})

#
# libdocument generation:
#

include_directories(include)
include_directories(src/googletest/googletest/include)

link_directories(lib)

add_library(libpython-static STATIC IMPORTED)
set_target_properties(libpython-static PROPERTIES
    IMPORTED_LOCATION src/python/libpython${PYTHON_VERSION}${PYTHON_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

# TODO Shared library build currently disabled due to linking problems with Python lib (not created at the moment).
# add_library(document SHARED ${libdocument_SRC})

add_library(document-static STATIC ${libdocument_SRC})
set_target_properties(document-static PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY lib
    LIBRARY_OUTPUT_DIRECTORY_DEBUG lib
    LIBRARY_OUTPUT_DIRECTORY_RELEASE lib
    IMPORT_LOCATION lib/${CMAKE_CFG_INTDIR}/libdocument${CMAKE_STATIC_LIBRARY_SUFFIX})

# Make sure headers are visible to the example executable below:
# libbson requires this two dirs: src/libbson/src/bson libbson-lib-prefix/src/libbson-lib-build/src/bson
include_directories(src/lib src/python src/python/Include)

# Dependencies:
add_dependencies(libpython-static python)
add_dependencies(document-static libpython-static)

# Library linking:
target_link_libraries(document-static libpython-static)

# Executables:
add_executable(libdocument-ex1 src/examples/ex1.c)
# Note: removed libbson-lib
# TODO: Why do I have to add 'python' and 'libpython-static' here?
add_dependencies(libdocument-ex1 python libpython-static document-static)

add_executable(tests ${test_SRC})
add_dependencies(tests document-static googletest-lib)

#
# Linking information for executables:
#

if(APPLE)
    find_library(INTL_LIB
    NAMES intl libintl
    HINTS "/usr/local/lib"
)
target_link_libraries(libdocument-ex1 document-static ${INTL_LIB})
else(APPLE)
target_link_libraries(libdocument-ex1 document-static dl m pthread util)
endif(APPLE)

# As suggested in the googletest README: link libgtest statically:
add_library(gtest STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION src/googletest/lib/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX})
if(APPLE)
target_link_libraries(tests document-static gtest ${INTL_LIB})
else(APPLE)
target_link_libraries(tests document-static dl m pthread util gtest)
endif(APPLE)

