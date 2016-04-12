.. Copyright 2016 Chris Drake

.. _installation:

****************
  Installation
****************

This chapter describes the various ways to install BoolExpr.

Supporting multiple platforms is a resource-intensive undertaking.
If you want support for your platform of choice,
please donate your resources in the form of a pull request with the necessary
documentation details.

Dependencies
============

First, you need a C++ compiler that supports the C++11 language standard.
The GCC (4.8.4) and Clang (3.4.0) versions that ship with Ubuntu Trusty should
work fine.

Second, you need the `Boost`_ development library.
To install it on Ubuntu::

   $ sudo apt-get install libboost-dev

If you are compiling from the source repository,
you will need the `CMake`_ build tool.
To install it on Ubuntu::

   $ sudo apt-get install cmake

Currently, the Python wrapper module only supports Python 3.4+.
To install it on Ubuntu::

   $ sudo apt-get install python3

Python Package Index
====================

To install BoolExpr from `PyPI`_ source distribution (recommended),
use `pip <https://pip.pypa.io>`_::

   $ pip install boolexpr

   Collecting boolexpr
     Using cached boolexpr-0.4.tar.gz
   Collecting cffi>=1.5.0 (from boolexpr)
     Using cached cffi-1.5.2.tar.gz
   Collecting pycparser (from cffi>=1.5.0->boolexpr)
     Using cached pycparser-2.14.tar.gz
   Installing collected packages: pycparser, cffi, boolexpr
     Running setup.py install for pycparser ... done
     Running setup.py install for cffi ... done
     Running setup.py install for boolexpr ... done
   Successfully installed boolexpr-0.4 cffi-1.5.2 pycparser-2.14

As you can see,
``pip`` and ``setuptools`` take care of installing the ``cffi`` and
``pycparser`` dependencies.

Source Code
===========

This is a bit more complicated than using PyPI,
because it requires you to build third party dependencies manually.

First,
clone the repository from GitHub using the ``recursive`` option to clone
sub-repositories::

   $ git clone --recursive https://github.com/cjdrake/boolexpr.git
   $ cd boolexpr

Next, build the library dependencies.
The `CryptoMiniSat`_ library is required::

   $ pushd third_party/cryptominisat
   $ git checkout 4.5.3
   $ cmake .
   $ make
   $ popd

Building `Google Test`_ is only required if you want to run the C++
unit test suite::

   $ pushd third_party/googletest
   $ git checkout release-1.7.0
   $ cmake .
   $ make
   $ popd

Python Wrapper Scripts
----------------------

While at the top-level directory of the repository,
first run the unit test suite to make sure everything is working::

   $ ./setup.py test

To install into your Python site-packages directory::

   $ ./setup.py install

.. _Boost: http://www.boost.org
.. _CMake: https://cmake.org
.. _CryptoMiniSat: https://github.com/msoos/cryptominisat
.. _Google Test: https://github.com/google/googletest
.. _PyPI: https://pypi.python.org/pypi
