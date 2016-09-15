cmake_minimum_required(VERSION 2.8.12.2)

project(boolexpr)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(VERSION 2.2)
set(BOOST_VERSION 1.54.0)

# Doxygen
include(FindDoxygen)

if(DOXYGEN_FOUND)
    configure_file(Doxyfile.in Doxyfile)
    add_custom_target(
        ccapiref
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)

file(GLOB boolexpr_srcs "include/boolexpr/*.h" "src/*.cc")
file(GLOB_RECURSE glucosamine_srcs "third_party/glucosamine/src/*.h" "third_party/glucosamine/src/core/Solver.cc")
file(GLOB_RECURSE boost_srcs "third_party/boost-${BOOST_VERSION}/*.hpp" "third_party/boost-${BOOST_VERSION}/*.cc")

add_library(boolexpr ${boolexpr_srcs} ${glucosamine_srcs})

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    target_compile_options(boolexpr PUBLIC -std=c++11 -Wall)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    target_compile_options(boolexpr PUBLIC -std=c++11 -Wall)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_compile_options(boolexpr PUBLIC /std:c++11 /Wall)
endif()

target_include_directories(boolexpr PUBLIC include)
target_include_directories(boolexpr PUBLIC third_party/boost-1.54.0)
target_include_directories(boolexpr PUBLIC third_party/glucosamine/src)

# Copy boolexpr library to python/boolexpr directory
add_custom_command(
    TARGET boolexpr
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:boolexpr>
        python/boolexpr/$<TARGET_FILE_NAME:boolexpr>
)

# Python bindings
add_subdirectory(python)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    # Google Test
    add_subdirectory(third_party/googletest)
    # Unit tests
    add_subdirectory(test)
endif()
