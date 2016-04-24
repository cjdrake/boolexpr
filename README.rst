***********************
  Boolean Expressions
***********************

.. image:: https://travis-ci.org/cjdrake/boolexpr.svg?branch=master
   :target: https://travis-ci.org/cjdrake/boolexpr

BoolExpr is a C++ library for symbolic Boolean algebra.

`Read the docs! <http://www.boolexpr.org>`_

System Dependencies
===================

This code is known to compile on Ubuntu Trusty Linux.
Install the following dependencies::

    sudo apt-get install -y build-essential cmake libboost-dev

Getting Started
===============

To get started using the code,
first clone the repository and its ``third_party`` dependencies::

    git clone --recursive https://github.com/cjdrake/boolexpr

Next, build the library dependencies.

To build `CryptoMiniSat <https://github.com/msoos/cryptominisat>`_::

    cd third_party/cryptominisat
    cmake .
    make

To build `Google Test <https://github.com/google/googletest>`_::

    cd third_party/googletest
    cmake .
    make

Run Tests
=========

After the dependencies have been built,
to run the functional test suite::

    make test

To collect code coverage data::

    make cover

The coverage report will be in ``build/cover/html/index.html``.

Build Documentation
===================

The documentation uses `Sphinx <http://www.sphinx-doc.org/en/stable>`_.
To build the html::

    pip install sphinx
    cd doc
    make html

Contributing
============

If you have some idea you would like to see realized,
please email me.
