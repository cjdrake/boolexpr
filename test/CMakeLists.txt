file(GLOB srcs "*.h" "*.cc")

add_executable(a.out ${srcs})
target_link_libraries(a.out boolexpr gtest)

# make do_test
add_custom_target(
    do_test
    ALL
    COMMAND a.out
    DEPENDS a.out
)

# make cover
if("${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
    include(CodeCoverage)
    setup_target_for_coverage(cover a.out html)
endif()
