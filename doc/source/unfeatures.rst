.. Copyright 2016 Chris Drake

.. _unfeatures:

**************
  Unfeatures
**************

This awkwardly-named chapter will discuss things included in the source code
that are not actually *features*,
but could be more accurately described as "tolerable problems".
The are purposefully not mentioned in the API reference.
Unfeatures are not guaranteed to be included in the future.
Use them at your own risk.

Python-only Functions
=====================

The following functions are only available in the Python boolexpr.py module:

* ``boolexpr.majority``
* ``boolexpr.achilles_heel``

They are the only components that are not currently implemented in C++.
The ``majority`` function uses syntax that is not compatible with Python 2.
Whether or not Python 2 is ever supported is an interesting question,
but ``boolexpr`` is a C++ library with a Python wrapper,
not a Python library mostly implemented in C++.

Preprocessor Usage
==================

In general, C++ preprocessor usage is evil.
As C++ continues evolving into a great programming language,
this kind of hackery should no longer be necessary.
Nothing is permanently ruled out,
but don't count on them until they are listed in the API reference.

BoolExpr Public Data and Methods
================================

The ``Literal::id`` constant is currently public because the poorly-implemented
CNF/DNF flattening algorithm relies on it for ordering.
There's a bug open to improve the flattening algorithm.
The ``id`` of a literal is an implementation detail.
The only thing the user interface should care about is the Variable name string.

The ``Illogical`` class is experimental.
It is supposed to represent an illogical condition such as metastability,
but the usage model is unclear right now.
