.. Copyright 2016 Chris Drake

.. _relnotes:

*****************
  Release Notes
*****************

Release 2.4
===========

Added a root context to the wrapper module so you can create variables without
having to understand what ``Context`` objects are for.

For example:

.. code-block:: pycon

   >>> from boolexpr import *
   >>> a = get_var('a')
   >>> w, x, y, z = map(get_var, "wxyz")
   >>> X = get_vars('x', 4, 4, 4)

Also, in the C++ API, changed the Array class so it uses ``unique_ptr`` instead
of raw pointers.
This makes memory management easier.

Fixed a couple serious issues with Python packaging:

* `Issue #19 <https://github.com/cjdrake/boolexpr/issues/19>`_
* `Issue #20 <https://github.com/cjdrake/boolexpr/issues/20>`_

Thanks to `shader <https://github.com/shader>`_ for pointing these out.

Release 2.3
===========

Now this is an exciting release!

First, the minor things.

The ``majority`` function now always returns a CNF.
There is no size advantage to DNF,
and CNF is almost always preferable for the purposes of SAT.

Also, there is a new ``NegativeOperator`` class.
Because inheritance allowed for some nice consolidation in this particular case.

Now onto the cool stuff.

We are swapping SAT solvers.
BoolExpr had been using
`CryptoMinisat <https://github.com/msoos/cryptominisat>`_.
Primarily due to build dependency complexities,
we are now using `Glucosamine <https://github.com/cjdrake/glucosamine>`_,
which is a fork of version 4.0 (2014) of the well-known
`Glucose <http://www.labri.fr/perso/lsimon/glucose/>`_ solver.
Both were derived from `MiniSAT <http://minisat.se/>`_ originally,
so I don't expect a drastic change in performance.

Another major achievement is the adoption of
`CMake <https://cmake.org/>`_ for build.
After looking around for quite a while,
this system appears to be the de-facto standard for C++ builds.

With all of this new-found cross-platform magic,
BoolExpr will now ship Python wheels for 1) Linux, 2) MacOS, and 3) Windows.
Getting this to work was a minor miracle,
but definitely worth it.

Release 2.2
===========

Added the following reduction methods to the ``ndarray`` class:

* ``nor_reduce``
* ``nand_reduce``
* ``xnor_reduce``

Also cleaned up some package metadata.

Release 2.1
===========

Added a ``to_dot`` method, and slightly improved the docs.

Release 2.0
===========

This is a major release that primarily fills out the array stuff
added in 1.3.
Lots of new documentation,
and several little bug fixes and other code cleanup.

Short list of new stuff:

* Absolute value for literals
* New ``BoolExpr.degree`` method
* New ndarray ``cat`` function
* New ndarray ``simplify`` method
* New C++ "term" iterator

Release 1.3
===========

This release contains most of the new stuff that's going into 2.0.
Most of it is still undocumented,
but it seemed complete enough to push out the door for testing purposes.

Short list of new stuff:

* faster XOR flattening
* New C++ ``Array`` data type, and Python wrapper
* ``iter_points`` function,
  for iterating through all points in a space
* ``mux`` function,
  for multiplexing an array of inputs.
* ``nhot`` function,
  which returns a CNF guaranteeing N bits are "hot".
* The ``boolexpr.ndarray`` N-dimensional array wrapper class,
  which adds a bunch of fancy Python whiz-bang to the ``Array`` type.
* ``zeros``, ``ones``, ``logicals``, ``uint2nda``, ``int2nda``,
  for creating arrays of constant values.
* ``array`` method,
  for a general-purpose ``ndarray`` factory function.
* ``Context.get_vars`` method,
  for creating N-dimensional arrays of variables.

Also lots of new improvements in testing, code coverage,
API documentation, and lint waivers.

Release 1.2
===========

Improved the performance of flattening.
In particular,
now you can flatten XOR with ten arguments within reasonable time.

Also, brought the ``nimpl``, and ``nite`` functions to the Python level.
I still like the way ``~impl`` and ``~ite`` look,
but these functions can be useful.

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
:meth:`boolexpr.BoolExpr.from_ast` methods to the
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
