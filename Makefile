# Filename: Makefile

CMAKE := cmake
COVERAGE := coverage
DOXYGEN := doxygen
GENHTML := genhtml
GSUTIL := gsutil
LCOV := lcov
PYLINT := pylint

CMSAT := third_party/cryptominisat
GTEST := third_party/googletest

# Currently, coverage only works with GCC
CXX := g++
CXXFLAGS := --std=c++11
LDFLAGS := -L$(CMSAT)/lib -L$(GTEST)
LDLIBS := -lcryptominisat4 -lgtest -lm4ri

GCS_BUCKET := gs://www.boolexpr.org
HTML_DIR := python/build/sphinx/html

.PHONY: help
help:
	@echo
	@echo "Usage: make [options] [target] ..."
	@echo
	@echo "Valid targets:"
	@echo
	@echo "    help                Display this help message"
	@echo
	@echo "    test                Run C++ unit tests"
	@echo "    cover               Collect C++ coverage"
	@echo
	@echo "    pytest              Run Python unit tests"
	@echo "    pylint              Run Python lint"
	@echo "    pycov               Collect Python coverage"
	@echo
	@echo "    html                Build Sphinx HTML documentation"
	@echo "    html-upload         Upload HTML to Google Cloud Storage"
	@echo
	@echo "    pypi-upload-src     Upload source to Python Package Index"
	@echo

.PHONY: test
test: build/test/a.out
	@./$<

.PHONY: do_lcov
do_lcov: build/cover/a.out
	@./$<
	@$(LCOV) -c -o build/cover/coverage.info -d build/cover
	@$(LCOV) -r build/cover/coverage.info "/usr/*" -o build/cover/coverage.info
	@$(LCOV) -r build/cover/coverage.info "third_party/*" -o build/cover/coverage.info

.PHONY: do_covall
do_covall: do_lcov
	@./script/covall.py build/cover/coverage.info

.PHONY: do_genhtml
do_genhtml: do_lcov
	@$(GENHTML) -o build/cover/html -t "BoolExpr Coverage" build/cover/coverage.info

.PHONY: cover
cover: do_genhtml

.PHONY: pytest
pytest:
	@cd python && ./setup.py test

.PHONY: pylint
pylint:
	@cd python && ( \
        ./setup.py build_ext -i && \
        $(PYLINT) boolexpr \
    )

.PHONY: pycov
pycov:
	@cd python && ( \
        ./setup.py build_ext -i && \
        $(COVERAGE) run test_boolexpr.py && \
        $(COVERAGE) html \
    )

.PHONY: html
html:
	@$(DOXYGEN)
	@cd python && ( \
        ./setup.py build_ext -i && \
        ./setup.py build_sphinx -s ../doc/source \
    )

.PHONY: html-upload
html-upload: html
	@$(GSUTIL) -m rsync -d -p -r $(HTML_DIR) $(GCS_BUCKET)
	@$(GSUTIL) -m acl ch -u AllUsers:R $(GCS_BUCKET)/**

.PHONY: pypi-upload-src
pypi-upload-src:
	@cd python && ( \
        ./setup.py sdist --formats=gztar,zip upload \
    )

#===============================================================================
# Source Code
#===============================================================================

BX_HDRS += include/boolexpr/boolexpr.h
BX_HDRS += src/argset.h
BX_HDRS += src/bxcffi.h

BX_SRCS += src/argset.cc
BX_SRCS += src/array.cc
BX_SRCS += src/binop.cc
BX_SRCS += src/boolexpr.cc
BX_SRCS += src/bxcffi.cc
BX_SRCS += src/compose.cc
BX_SRCS += src/constants.cc
BX_SRCS += src/context.cc
BX_SRCS += src/count.cc
BX_SRCS += src/dot.cc
BX_SRCS += src/equivalent.cc
BX_SRCS += src/flatten.cc
BX_SRCS += src/invert.cc
BX_SRCS += src/iter.cc
BX_SRCS += src/latop.cc
BX_SRCS += src/nnf.cc
BX_SRCS += src/operators.cc
BX_SRCS += src/posop.cc
BX_SRCS += src/restrict.cc
BX_SRCS += src/sat.cc
BX_SRCS += src/simplify.cc
BX_SRCS += src/tostr.cc
BX_SRCS += src/tseytin.cc

TEST_HDRS += test/boolexprtest.h

TEST_SRCS += test/array_test.cc
TEST_SRCS += test/basic_test.cc
TEST_SRCS += test/binop_test.cc
TEST_SRCS += test/boolexprtest.cc
TEST_SRCS += test/bxcffi_test.cc
TEST_SRCS += test/compose_test.cc
TEST_SRCS += test/count_test.cc
TEST_SRCS += test/dot_test.cc
TEST_SRCS += test/flatten_test.cc
TEST_SRCS += test/iter_test.cc
TEST_SRCS += test/nnf_test.cc
TEST_SRCS += test/posop_test.cc
TEST_SRCS += test/sat_test.cc
TEST_SRCS += test/simplify_test.cc
TEST_SRCS += test/tostr_test.cc
TEST_SRCS += test/tseytin_test.cc
TEST_SRCS += test/main.cc

#===============================================================================
# Third Party Libraries
#===============================================================================

$(CMSAT)/lib/libcryptominisat4.a:
	@cd $(CMSAT) && $(CMAKE) . && $(MAKE)

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
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(CMSAT)/include -Iinclude -Isrc $<

build/test/%.o: test/%.cc $(BX_HDRS) $(TEST_HDRS) | build/test/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(CMSAT)/include -I$(GTEST)/include -Iinclude -Itest $<

BLD_TEST_OBJS += $(patsubst src/%.cc,build/test/%.o,$(BX_SRCS))
BLD_TEST_OBJS += $(patsubst test/%.cc,build/test/%.o,$(TEST_SRCS))

build/test/a.out: $(BLD_TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g -pthread -static $(LDFLAGS) $^ $(LDLIBS)

# Coverage
build/cover/: | build/
	@mkdir $@

build/cover/%.o: src/%.cc $(BX_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g --coverage -I$(CMSAT)/include -Iinclude -Isrc $<

build/cover/%.o: test/%.cc $(BX_HDRS) $(TEST_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(CMSAT)/include -I$(GTEST)/include -Iinclude -Itest $<

BLD_COVER_OBJS += $(patsubst src/%.cc,build/cover/%.o,$(BX_SRCS))
BLD_COVER_OBJS += $(patsubst test/%.cc,build/cover/%.o,$(TEST_SRCS))

build/cover/a.out: $(BLD_COVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g --coverage -pthread -static $(LDFLAGS) $^ $(LDLIBS)
