# Copyright (c) 2012 - 2015, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_program(lcov_path lcov)
find_program(genhtml_path genhtml)

set(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE
)

set(CMAKE_C_FLAGS_COVERAGE
    "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE
)

set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE
)

set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE
)

# Parameters:
#     _targetname : The name of new the custom make target
#
#     _testrunner : The name of the target which runs the tests.
#                   MUST return ZERO always, even on errors.
#                   If not, no coverage report will be created!
#
#     _outputname : lcov output is generated as _outputname.info
#                   HTML report is generated in _outputname/index.html
#
# NOTE: Automatically excludes coverage from /usr/*, test/*, and third_party/*

function(setup_target_for_coverage
    _targetname
    _testrunner
    _outputname
)
    if(NOT lcov_path)
        message(FATAL_ERROR "lcov not found!")
    endif()

    if(NOT genhtml_path)
        message(FATAL_ERROR "genhtml not found!")
    endif()

    set(coverage_info "${CMAKE_BINARY_DIR}/${_outputname}.info")
    set(coverage_cleaned "${coverage_info}.cleaned")

    separate_arguments(test_command UNIX_COMMAND "${_testrunner}")

    # Setup target
    add_custom_target(
        ${_targetname}

        # Cleanup lcov
        ${lcov_path} --directory . --zerocounters

        # Run tests
        COMMAND ${test_command} ${ARGV3}

        # Capture lcov counters and generate report
        COMMAND ${lcov_path} --directory . --capture --output-file ${coverage_info}
        COMMAND ${lcov_path} --remove ${coverage_info} '/usr/*' 'test/*' 'third_party/*' --output-file ${coverage_cleaned}
        COMMAND ${genhtml_path} -o ${_outputname} ${coverage_cleaned}
        COMMAND ${CMAKE_COMMAND} -E remove ${coverage_info} ${coverage_cleaned}

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # Show report location
    add_custom_command(
        TARGET ${_targetname} POST_BUILD
        COMMAND ;
        COMMENT "Report: ${CMAKE_BINARY_DIR}/${_outputname}/index.html"
    )
endfunction() # setup_target_for_coverage
