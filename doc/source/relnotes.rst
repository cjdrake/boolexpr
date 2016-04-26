.. Copyright 2016 Chris Drake

.. _relnotes:

*****************
  Release Notes
*****************

Release 1.1
===========

Fixed `Issue 15 <https://github.com/cjdrake/boolexpr/issues/15>`_.

Lots of other miscellaneous refactoring.

Release 1.0
===========

Yay, this is good enough to stamp as 1.0.
Most of the changes since 0.4 were to firm up questionable design decisions,
and add documentation for anything and everything.

The only new feature was to add the
:meth:`boolexpr.BoolExpr.to_ast` and
:meth:`boolexpr.BoolExpr.from-ast` methods to the
:class:`boolexpr.BoolExpr` class.

Release 0.4
===========

Improved the various C++ iterator capabilities.
Added ``iter_cfs`` and ``iter_domain`` iterator methods.
Added ``smoothing``, ``consensus``, and ``derivative`` methods.
Lots of miscellaneous documentation updates,
thought this area still needs some work.
Implemented Python ``to_ast`` and ``from_ast`` methods,
to make it possible to convert BoolExpr objects to something pickleable.

Release 0.3
===========

Lots of improvements to the foreign function interface.

Implemented a SAT iterator,
so now you can iterate through all satisfying input points.

Constants and literals in the Python module now behave like singletons.
The FFI layer returns a new wrapper object,
but if the Python layer has a copy of the object already,
it uses that copy, and drops the wrapper object.
This makes lots of simple comparison operations much easier.

Release 0.2
===========

This release fixes a couple memory leak issues with ``0.1``,
and introduces the capability to simplify a Boolean expression with an
"unknown" input value.

Release 0.1
===========

This is a "hello world" release,
to see if all the pieces are working with
`PyPI <https://pypi.python.org/pypi>`_.
