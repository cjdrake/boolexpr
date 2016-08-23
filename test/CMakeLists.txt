include(CodeCoverage)

# a.out
file(GLOB srcs "*.h" "*.cc")
add_executable(a.out ${srcs})
target_link_libraries(a.out boolexpr glucosamine gtest)

# make test
add_custom_target(
    do_test
    COMMAND a.out
    DEPENDS a.out
)

# make cover
setup_target_for_coverage(cover a.out html)
