# Filename: Makefile

CMAKE := cmake
GENHTML := genhtml
LCOV := lcov

CXXFLAGS := --std=c++11

.PHONY: help
help:
	@printf "Usage: make [options] [target] ...\n"
	@printf "\n"
	@printf "Valid targets:\n"
	@printf "\n"
	@printf "    help            Display this help message\n"
	@printf "    test            Run unit tests\n"
	@printf "    cover           Collect coverage\n"

.PHONY: test
test: build/test/a.out
	@./$<

do_lcov: build/cover/a.out
	@./$<
	@$(LCOV) -c -o build/cover/coverage.info -d build/cover
	@$(LCOV) -r build/cover/coverage.info "/usr/*" -o build/cover/coverage.info
	@$(LCOV) -r build/cover/coverage.info "third_party/*" -o build/cover/coverage.info

do_covall: do_lcov
	@./script/covall.py build/cover/coverage.info

do_genhtml: do_lcov
	@$(GENHTML) -o build/cover/html -t "BoolExpr Coverage" build/cover/coverage.info

.PHONY: cover
cover: do_genhtml

#===============================================================================
# Source Code
#===============================================================================

BX_HDRS := include/boolexpr/boolexpr.h
BX_SRCS := \
    src/argset.cc \
    src/binop.cc \
    src/boolexpr.cc \
    src/compose.cc \
    src/context.cc \
    src/count.cc \
    src/equivalent.cc \
    src/flatten.cc \
    src/invert.cc \
    src/iter.cc \
    src/nnf.cc \
    src/pushdown_not.cc \
    src/sat.cc \
    src/simplify.cc \
    src/tseytin.cc \

TEST_HDRS := test/boolexprtest.h
TEST_SRCS := \
    test/basic_test.cc \
    test/binop_test.cc \
    test/boolexprtest.cc \
    test/compose_test.cc \
    test/count_test.cc \
    test/flatten_test.cc \
    test/nnf_test.cc \
    test/pushdown_not_test.cc \
    test/sat_test.cc \
    test/simplify_test.cc \
    test/tseytin_test.cc \
    test/main.cc \

#===============================================================================
# Third Party
#===============================================================================

CMSAT := third_party/cryptominisat

$(CMSAT)/lib/libcryptominisat4.a:
	@cd $(CMSAT) && $(CMAKE) . && $(MAKE)

GTEST := third_party/googletest

$(GTEST)/libgtest.a:
	@cd $(GTEST) && $(CMAKE) . && $(MAKE)

#===============================================================================
# Build Rules
#===============================================================================

build/:
	@mkdir $@

# Unit Tests
build/test/: | build/
	@mkdir $@

build/test/%.o: src/%.cc $(BX_HDRS) | build/test/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(CMSAT)/include -Iinclude $<

build/test/%.o: test/%.cc $(BX_HDRS) $(TEST_HDRS) | build/test/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(GTEST)/include -Iinclude -Itest $<

BLD_TEST_OBJS := \
    $(patsubst src/%.cc,build/test/%.o,$(BX_SRCS)) \
    $(patsubst test/%.cc,build/test/%.o,$(TEST_SRCS))

build/test/a.out: $(BLD_TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g -pthread -static -L$(CMSAT)/lib -L$(GTEST) $^ -lcryptominisat4 -lgtest

# Coverage
build/cover/: | build/
	@mkdir $@

build/cover/%.o: src/%.cc $(BX_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g --coverage -I$(CMSAT)/include -Iinclude $<

build/cover/%.o: test/%.cc $(BX_HDRS) $(TEST_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(GTEST)/include -Iinclude -Itest $<

BLD_COVER_OBJS := \
    $(patsubst src/%.cc,build/cover/%.o,$(BX_SRCS)) \
    $(patsubst test/%.cc,build/cover/%.o,$(TEST_SRCS))

build/cover/a.out: $(BLD_COVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g --coverage -pthread -static -L$(CMSAT)/lib -L$(GTEST) $^ -lcryptominisat4 -lgtest
