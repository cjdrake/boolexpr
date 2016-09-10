cmake_minimum_required(VERSION 2.8.12.2)

project(boolexpr)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(VERSION 2.2)

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

# Glucosamine
add_subdirectory(third_party/glucosamine)

file(GLOB srcs "include/boolexpr/*.h" "src/*.cc")

add_library(boolexpr ${srcs})
target_compile_options(boolexpr PUBLIC -std=c++11 -Wall)
target_include_directories(boolexpr PUBLIC include)
target_include_directories(boolexpr PUBLIC third_party/boost-1.54.0)
target_link_libraries(boolexpr glucosamine)

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
