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

Build Tools
===========

First, you need a C++ compiler that supports the C++11 language standard.
The GCC (4.8.4) and Clang (3.4.0) versions that ship with Ubuntu Trusty should
work fine.

If you are compiling from the source repository,
you will need the CMake_ build tool.
To install it on Ubuntu::

   $ sudo apt-get install cmake

Currently, the Python wrapper module only supports Python 3.4+.
To install it on Ubuntu::

   $ sudo apt-get install python3

Python Package Index
====================

To install BoolExpr from PyPI_ source distribution (recommended),
use pip_::

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
pip_ and setuptools_ take care of installing the CFFI_ and
pycparser_ dependencies.

Source Code
===========

This is a bit more complicated than using PyPI,
because it requires you to build third party dependencies manually.

First,
clone the repository from GitHub using the ``recursive`` option to clone
sub-repositories::

   $ git clone --recursive https://github.com/cjdrake/boolexpr
   $ cd boolexpr

Next, use CMake_ to create the build files::

   $ mkdir build
   $ cd build
   $ cmake ..

To run the C++ unit test suite::

   $ make do_test

To install the headers and libraries locally::

   $ make install

Python Wrapper Scripts
----------------------

From the ``build`` directory, change to the ``python`` subdirectory.

To run the Python unit test suite::

   $ ./setup.py test

To install into your Python site-packages directory::

   $ ./setup.py install

.. _CFFI: https://cffi.readthedocs.org
.. _CMake: https://cmake.org
.. _pip: https://pip.pypa.io
.. _pycparser: https://github.com/eliben/pycparser
.. _PyPI: https://pypi.python.org/pypi
.. _setuptools: https://setuptools.readthedocs.io/en/latest
