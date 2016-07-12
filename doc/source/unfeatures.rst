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

Python-only Components
======================

The following functions are only available in the Python boolexpr package:

* ``boolexpr.nhot``
* ``boolexpr.majority``
* ``boolexpr.achilles_heel``
* ``boolexpr.mux``
* ``boolexpr.exists``
* ``boolexpr.forall``

They are "miscellaneous" components, which are not currently implemented in C++.
The ``majority`` function returns a disjunctive normal form expression by default.
This is due to the slightly smaller size of the DNF.
However, the importance of CNF to SAT might change this convention in the future.

The ``boolexpr.ndarray`` data type is also entirely implemented in Python.
This is partly out of convenience.
Multi-dimensional arrays are very clunky in C++,
but Python (thanks to ``numpy``) has fantastic resources available for
beautiful implementation of MDA-type objects.

Preprocessor Usage
==================

In general, C++ preprocessor usage is evil.
As C++ continues evolving into a great programming language,
this kind of hackery should no longer be necessary.
Nothing is permanently ruled out,
but don't count on them until they are listed in the API reference.

Boost Usage
===========

Currently, the implementation uses ``boost/optional.hpp`` because that feature
is not part of the C++11 standard.
Since it *will* be part of the C++14 standard,
expect that dependency to disappear at some point in the future.

BoolExpr Public Data and Methods
================================

The ``Literal::id`` constant is currently public because the poorly-implemented
CNF/DNF flattening algorithm relies on it for ordering.
There is a bug open to improve the flattening algorithm.
The ``id`` of a literal is an implementation detail.
The only thing the user interface should care about is the ``Variable``
name string.

The ``Illogical`` class is experimental.
It is supposed to represent an illogical condition such as metastability,
but the usage model is unclear right now.
