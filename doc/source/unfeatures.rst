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

* ``boolexpr.onehot0``
* ``boolexpr.onehot``
* ``boolexpr.majority``
* ``boolexpr.achilles_heel``

They are the only components that are not currently implemented in C++.
The first three also use syntax that is not compatible with Python 2.
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

The ``BoolExpr::atom_count`` and ``BoolExpr::op_count`` methods are
1) almost useless, and 2) ambiguous.
The depth and size of an expression tree are more interesting than counting
operators and atoms.
The number of literals is currently counted by walking through the tree and
incrementing the count whenever it observes a literal.
But literals are singletons.
So, for example, if the variable ``x`` is observed ten times at the leaf-level,
even though there's only one literal ``x``, it is counted ten times.
The same is true of operators to some extent.
It is possible for a particular sub-expression to be used multiple times in an
expression tree.

The ``Literal::id`` constant is currently public because the poorly-implemented
CNF/DNF flattening algorithm relies on it for ordering.
There's a bug open to improve the flattening algorithm.
The ``id`` of a literal is an implementation detail.
The only thing the user interface should care about is the Variable name string.
