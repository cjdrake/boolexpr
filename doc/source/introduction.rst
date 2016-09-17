.. Copyright 2016 Chris Drake

.. _introduction:

****************
  Introduction
****************

BoolExpr is an open source C++ library for symbolic Boolean algebra.
It implements a C foreign function interface (FFI),
and has a Python binding.

The underlying data structure is a directed, acyclic graph.
Leaf nodes are referred to as *atoms*,
and branch nodes are referred to as *operators*.
With the exception of unknown constants (similar to Boolean "don't care"),
graph nodes have both a negative and positive form.
So instead of having a separate "NOT" operator,
you have NOR/OR, NAND/AND, XNOR/XOR, etc.
This also applies to *literals*,
the name given to a Boolean variable or its complement.

Do not confuse this implementation with Randal Bryant's seminal work on
symbolic Boolean manipulation of ordered decision diagrams [1]_.
They are very different things.

The majority of this documentation is oriented to the Python API,
because the Python REPL is a nice way to demonstrate concepts.

Features
========

* Integration with the advanced, high performance Glucosamine_
  SAT solver library
* Tseytin transformation
* Formal equivalence checking
* Expression simplification
* Optimistic X propagation

Dependencies
============

This section lists all of BoolExpr's dependencies.

If you want to compile from source,
you will need the CMake_ build system installed.

The following third party libraries are included with the repository:

* Boost_, for some internal data types
* Glucosamine_, for SAT solving

There are a few ways to install the Python package,
but I recommend using a `virtual environment`_ and pip_.
Running ``pip install boolexpr`` will install the following dependencies:

* CFFI_, for the Python wrapper
* pycparser_, a C99 parser

The following tools are required for development and testing:

* `Coverage.py`_, for Python coverage measurement
* Doxygen_, for building C++ API reference
* `Google Test`_, for C++ unit testing
* LCOV_, for C++ coverage measurement
* Pylint_, for Python linting
* Sphinx_, for building documentation

Influences
==========

The design of BoolExpr has been influenced by many things.

The original inspiration
(circa `2012 <https://groups.google.com/forum/#!topic/sympy/KUBcm5iGSkQ>`_)
was Sympy_,
a much more comprehensive symbolic mathematics library.
Sympy is a pure Python library,
so it isn't free to pursue some performance optimizations that are only
possible when using C/C++.

Other notable inspirations are the Boolean libraries in Maple_
and Mathematica_.
Their online documentation can be useful as a guide,
but the author does not have a license for these commercial tools.

Mostly,
the implementation of BoolExpr has been derived from PyEDA_.
Its C ``boolexpr`` library had some value,
so it was pulled out as an independent library.
The author decided to migrate from C to C++ to take advantage of the new C++11
language features,
and the impressive landscape of tooling and libraries.
Since BoolExpr has a much narrower focus than PyEDA_,
it is possible to correct some of the klunky design decisions,
and focus like a laser on performance.

Goals
=====

BoolExpr exists for its own sake.
It could accurately be described as a
`yak shaving <https://en.wiktionary.org/wiki/yak_shaving>`_ exercise.
One day it might be useful for something,
but until that time its main goal is educational.
Use it to learn about Boolean algebra,
C/C++/Python, software engineering, CFFI_ usage, etc.

Free Software
=============

BoolExpr is free software.
You can use, reproduce, and distribute it under the terms of the
Apache-2.0_ software license.

References
==========

.. [1] `R. Bryant, Symbolic Boolean Manipulation with Ordered Binary Decision Diagrams, 1992 <http://dl.acm.org/citation.cfm?id=136043>`_

.. _Apache-2.0: http://www.apache.org/licenses/LICENSE-2.0
.. _Boost: http://www.boost.org
.. _CFFI: https://cffi.readthedocs.org
.. _CMake: https://cmake.org
.. _Coverage.py: https://coverage.readthedocs.org
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen
.. _Glucosamine: https://github.com/cjdrake/glucosamine
.. _Google Test: https://github.com/google/googletest
.. _LCOV: http://ltp.sourceforge.net/coverage/lcov.php
.. _Maple: http://maplesoft.com
.. _Mathematica: https://www.wolfram.com/mathematica
.. _pip: https://pip.pypa.io
.. _pycparser: https://github.com/eliben/pycparser
.. _PyEDA: https://pyeda.readthedocs.org
.. _Pylint: https://www.pylint.org
.. _setuptools: https://setuptools.readthedocs.io/en/latest
.. _Sphinx: http://www.sphinx-doc.org
.. _Sympy: http://docs.sympy.org
.. _virtual environment: https://docs.python.org/3/library/venv.html
