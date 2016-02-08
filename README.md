# Boolean Expressions

BoolExpr is a C++ library for symbolic Boolean algebra.

[Read the docs!](http://www.boolexpr.org)

# System Dependencies

This code is known to compile on Ubuntu Trusty Linux.
Install the following dependencies:

    sudo apt-get install -y build-essential cmake libboost-dev

# Getting Started

To get started using the code,
first clone the repository and its `third_party` dependencies:

    git clone --recursive https://github.com/cjdrake/boolexpr

Next, build the library dependencies.

To build [cryptominisat](https://github.com/msoos/cryptominisat):

    cd third_party/cryptominisat
    cmake .
    make

To build [google test](https://github.com/google/googletest):

    pushd third_party/googletest
    cmake .
    make

# Run Tests

After the dependencies have been built,
to run the functional test suite:

    make test

To collect code coverage data:

    make cover

The coverage report will be in `build/cover/html/index.html`.

# Build Documentation

The documentation uses [Sphinx](http://www.sphinx-doc.org/en/stable).
To build the html:

    pip install sphinx
    cd doc
    make html

# Contributing

This software is undocumented, unoptimized for performance, feature-poor,
and generally incompetently constructed.

If you have some idea you would like to see realized,
please email me.
