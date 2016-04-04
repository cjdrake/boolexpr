.. Copyright 2016 Chris Drake

*****************
  Release Notes
*****************

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
