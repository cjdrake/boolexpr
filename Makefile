# Filename: Makefile

CMAKE := cmake
COVERAGE := coverage
DOXYGEN := doxygen
GENHTML := genhtml
GSUTIL := gsutil
LCOV := lcov

CMSAT := third_party/cryptominisat
GTEST := third_party/googletest

# Currently, coverage only works with GCC
CXX := g++
CXXFLAGS := --std=c++11
LDFLAGS := -L$(CMSAT)/lib -L$(GTEST)
LDLIBS := -lcryptominisat4 -lgtest -lm4ri

GCS_BUCKET := gs://www.boolexpr.org
HTML_DIR := build/sphinx/html

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
	@echo "    pypi-upload-whl     Upload wheel to Python Package Index"
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
	@./setup.py test

.PHONY: pylint
pylint:
	@./setup.py build_ext -i
	@pylint boolexpr

.PHONY: pycov
pycov: test_boolexpr.py
	@./setup.py build_ext -i
	@coverage run test_boolexpr.py
	@coverage html

.PHONY: html
html:
	@$(DOXYGEN)
	@./setup.py build_ext -i
	@./setup.py build_sphinx

.PHONY: html-upload
html-upload: html
	@$(GSUTIL) -m rsync -d -p -r $(HTML_DIR) $(GCS_BUCKET)
	@$(GSUTIL) -m acl ch -u AllUsers:R $(GCS_BUCKET)/**

.PHONY: pypi-upload-src
pypi-upload-src:
	@./setup.py sdist --formats=gztar,zip upload

.PHONY: pypi-upload-whl
pypi-upload-whl:
	@./setup.py bdist_wheel upload

#===============================================================================
# Source Code
#===============================================================================

BX_HDRS := \
    include/boolexpr/boolexpr.h \
    src/argset.h \
    src/bxcffi.h \

BX_SRCS := \
    src/argset.cc \
    src/array.cc \
    src/binop.cc \
    src/boolexpr.cc \
    src/bxcffi.cc \
    src/compose.cc \
    src/constants.cc \
    src/context.cc \
    src/count.cc \
    src/dot.cc \
    src/equivalent.cc \
    src/flatten.cc \
    src/invert.cc \
    src/iter.cc \
    src/latop.cc \
    src/nnf.cc \
    src/operators.cc \
    src/posop.cc \
    src/restrict.cc \
    src/sat.cc \
    src/simplify.cc \
    src/tostr.cc \
    src/tseytin.cc \

TEST_HDRS := test/boolexprtest.h
TEST_SRCS := \
    test/array_test.cc \
    test/basic_test.cc \
    test/binop_test.cc \
    test/boolexprtest.cc \
    test/bxcffi_test.cc \
    test/compose_test.cc \
    test/count_test.cc \
    test/dot_test.cc \
    test/flatten_test.cc \
    test/iter_test.cc \
    test/nnf_test.cc \
    test/posop_test.cc \
    test/sat_test.cc \
    test/simplify_test.cc \
    test/tostr_test.cc \
    test/tseytin_test.cc \
    test/main.cc \

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

BLD_TEST_OBJS := \
    $(patsubst src/%.cc,build/test/%.o,$(BX_SRCS)) \
    $(patsubst test/%.cc,build/test/%.o,$(TEST_SRCS))

build/test/a.out: $(BLD_TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g -pthread -static $(LDFLAGS) $^ $(LDLIBS)

# Coverage
build/cover/: | build/
	@mkdir $@

build/cover/%.o: src/%.cc $(BX_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g --coverage -I$(CMSAT)/include -Iinclude -Isrc $<

build/cover/%.o: test/%.cc $(BX_HDRS) $(TEST_HDRS) | build/cover/
	$(CXX) $(CXXFLAGS) -o $@ -c -g -I$(CMSAT)/include -I$(GTEST)/include -Iinclude -Itest $<

BLD_COVER_OBJS := \
    $(patsubst src/%.cc,build/cover/%.o,$(BX_SRCS)) \
    $(patsubst test/%.cc,build/cover/%.o,$(TEST_SRCS))

build/cover/a.out: $(BLD_COVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ -g --coverage -pthread -static $(LDFLAGS) $^ $(LDLIBS)
