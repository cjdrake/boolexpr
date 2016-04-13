.. Copyright 2016 Chris Drake

.. _quickstart:

***************
  Quick Start
***************

This chapter will get you up and running with BoolExpr.
We will discuss how to create Boolean expressions,
and some of their most essential properties.

See the :ref:`installation` chapter for how to install BoolExpr.
We will assume you have installed the library,
and have executed the following in your Python REPL of choice
(recommend using `IPython <https://ipython.org>`_).

.. code-block:: pycon

   >>> import boolexpr as bx

Creating Variables
==================

In order to construct Boolean functions,
we must first create some symbolic variables.

Let's create four symbols: :math:`w, x, y, z`.

.. code-block:: pycon

   >>> ctx = bx.Context()
   >>> w = ctx.get_var("w")
   >>> type(w)
   boolexpr.Variable
   >>> x, y, z = map(ctx.get_var, "xyz")

Variable Context
----------------

If you are familiar with other symbolic algebra systems,
the first statement ``ctx = bx.Context()`` will be confusing.

A variable "context" is a kind of namespace container for Boolean variables.
It manages the creation and storage of variables.
No two variables within the same context can have the same name.
Therefore, asking a ``Context`` object for the same name will always return
the same object.

.. code-block:: pycon

   >>> ctx.get_var("foo") is ctx.get_var("bar")
   False
   >>> ctx.get_var("foo") is ctx.get_var("foo")
   True

Variable Naming
---------------

You can name variables any string that has an ASCII encoding.
(since the C++ layer uses ``std::string`` for the variable name type).

The following are all valid variable names:

.. code-block:: pycon

   >>> x = ctx.get_var("x")
   >>> x_0 = ctx.get_var("x_0")
   >>> z = ctx.get_var("Zebra")
   >>> rbbb = ctx.get_var("Rubber Baby Buggy Bumpers")
   >>> hrm = ctx.get_var("This !@#$ is %^&* legal -+=| too")

But using Unicode will throw an exception.

.. code-block:: pycon

   >>> nope = ctx.get_var("\u0394")
   UnicodeEncodeError: 'ascii' codec can't encode character '\u0394'
                       in position 0: ordinal not in range(128)

Tips and Tricks
---------------

It's a common operation to create more than one variable at a time.
BoolExpr does not provide any assistance (yet),
but Python's list comprehensions can come in handy.

For example,
to create a list of eight :math:`x` variables with an index:

.. code-block:: pycon

   >>> xs = [ctx.get_var("x_%s" % i) for i in range(8)]
   >>> xs
   [x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7]
   >>> xs[3]
   x_3

Creating multi-dimensional arrays is a bit trickier,
but possible.
To create a 4x4 list of :math:`x` variables:

.. code-block:: pycon

   >>> xs = [[ctx.get_var("x[%s,%s]" % (i,j)) for j in range(4)] for i in range(4)]
   >>> xs
   [[x[0,0], x[0,1], x[0,2], x[0,3]],
    [x[1,0], x[1,1], x[1,2], x[1,3]],
    [x[2,0], x[2,1], x[2,2], x[2,3]],
    [x[3,0], x[3,1], x[3,2], x[3,3]]]
   >>> xs[2][3]
   x[2,3]

Creating Expressions
====================

This section covers the various ways to construct expression trees.

Overloaded Python Operators
---------------------------

BoolExpr overloads the Python "bitwise" operators,
``~ | & ^`` to mean NOT, OR, AND, and XOR, respectively.
This allows you to construct expressions with the most common logical operators
in a domain specific language.

For example,
the following code will create an expression, ``f``:

.. code-block:: pycon

   >>> f = ~w | x & ~y ^ z
   >>> f
   Or(~w, Xor(And(x, ~y), z))

The name ``f`` is a Python handle.
The expression object itself is just a pointer,
and has no intrinsic name.

In graphical form, the function ``f`` looks like this:

.. graphviz::

   digraph g {
       node [shape=doublecircle] f
       node [shape=circle]
       f -> or
       not_w [label="~w"]
       or -> not_w
       or -> xor
       xor -> and
       xor -> z
       and -> x
       not_y [label="~y"]
       and -> not_y
   }

Constant Inputs
---------------

To use constant zero and one atoms,
use either ``False/True``, or ``0/1`` in the expression.

.. code-block:: pycon

   >>> w | False
   Or(w, 0)
   >>> True & x
   And(1, x)
   >>> 0 ^ y ^ 1
   Xor(Xor(0, y), 1)

Zero and one are singletons within the ``boolexpr`` module.
If you really need access to them for some reason,
use the ``ZERO`` and ``ONE`` handles.

.. code-block:: pycon

   >>> bx.ZERO | bx.ONE
   Or(0, 1)

There is also a constant called "logical".
It represents a constant value of either zero or one.
Since there is no handy Python analog to this value,
you can use either ``'x'``, ``'X'``, or ``LOGICAL`` as a fill-in.
The notation ``X`` comes from Verilog four-state logic.

.. code-block:: pycon

   >>> w | 'X'
   Or(w, X)
   >>> bx.LOGICAL & w
   And(X, w)

Equal, Implies, and If-Then-Else
--------------------------------

BoolExpr supports the "unequal, "equal", "implies" and "if-then-else"
symbolic operators.
Python does not have good symbols for us to use for these,
so you must use the ``neq``, ``eq``, ``impl`` and ``ite`` functions.

.. code-block:: pycon

   >>> bx.neq(~y, z)
   Unequal(~y, z)
   >>> bx.eq(y, ~z)
   Equal(y, ~z)
   >>> p, q = map(ctx.get_var, "pq")
   >>> bx.impl(p, q)
   Implies(p, q)
   >>> s, d1, d0 = map(ctx.get_var, "s d1 d0".split())
   >>> bx.ite(s, d1, d0)
   IfThenElse(s, d1, d0)

The rules for constants are the same as in the previous section.

.. code-block:: pycon

   >>> bx.impl(p, False)
   Implies(p, 0)
   >>> bx.ite(True, d1, 'X')
   IfThenElse(1, d1, X)

Nary Operator Functions
-----------------------

One disadvantage of using Python's builtin operators is that they only
allow you to create binary trees.
But the OR, AND, and XOR operators are N-ary operators,
which means they take an arbitrary number of arguments, :math:`N`.

To construct expressions with flat, N-ary operators,
use the ``or_``, ``and_``, and ``xor`` functions.

.. code-block:: pycon

   >>> bx.or_(w, x, y, z)
   Or(w, x, y, z)
   >>> bx.and_(w, False, y, True)
   And(w, 0, y, 1)
   >>> bx.xor(w|x, y&z, bx.impl(p,q))
   Xor(Or(w, x), And(y, z), Implies(p, q))

In addition,
the ``nor``, ``nand``, and ``xnor`` functions provide the "negative"
form of these N-ary operators.

Simplification
--------------

In the previous sections,
you may have noticed places where we created Boolean expressions with obvious
simplifications.
For example, we know that :math:`x \cdot 0 \iff 0`,
but writing out that equation will produce the following:

.. code-block:: pycon

   >>> x & False
   And(x, 0)

BoolExpr purposefully does not automatically simplify these expressions by
default,
but you can use the ``simplify`` method to get the more obvious output.

.. code-block:: pycon

   >>> f = x & False
   >>> f.simplify()
   0

The ``simplify`` method attempts to perform all sorts of transformations
with the goal of getting rid of constants,
and sub-expressions that can easily be proven equivalent to constants.

Sometimes,
you might wish for the default behavior to automatically simplify.
For this, every operator function has a corresponding auto-simplify form:

==========  ================
 Basic Op    Simplifying Op
==========  ================
 ``nor``     ``nor_s``
 ``or_``     ``or_s``
 ``nand``    ``nand_s``
 ``and_``    ``and_s``
 ``xnor``    ``xnor_s``
 ``xor``     ``xor_s``
 ``neq``     ``neq_s``
 ``eq``      ``eq_s``
 ``impl``    ``impl_s``
 ``ite``     ``ite_s``
==========  ================

Function Domain and Range
=========================

The most basic way to understand a Boolean function is to examine its
"truth table",
a list of how all possible input assignments map to output assignments.

First,
given some arbitrary expression ``f``,
what variables does it depend on?
This set is often called the *support* set of the function.
To get it, use the ``support()`` method:

.. code-block:: pycon

   >>> f = ~w | x & ~y ^ z
   >>> f.support()
   {y, w, z, x}

A Boolean function is a rule that maps points in an :math:`N`-dimensional
Boolean space to an element in :math:`\{0, 1\}`.
In formal mathematical lingo, :math:`f: B^N \Rightarrow B`,
where :math:`B^N` means the Cartesian product of :math:`N` sets of type
:math:`\{0, 1\}`.
For example, if you have three input variables, :math:`a, b, c`,
each defined on :math:`\{0, 1\}`,
then :math:`B^3 = \{0, 1\}^3 = \{(0, 0, 0), (0, 0, 1), (0, 1, 0), (0, 1, 1), (1, 0, 0), (1, 0, 1), (1, 1, 0), (1, 1, 1)\}`.
:math:`B^3` is the **domain** of the function (the input part),
and :math:`B = \{0, 1\}` is the **range** of the function (the output part).

Use the ``iter_domain`` generator method to iterate through all points in
the domain,
The ``restrict`` method evaluates the output value of a function at one
particular input point.
The combination of these two methods produces a truth table:

.. code-block:: pycon

   >>> for point in f.iter_domain():
           print(point, f.restrict(point))
   {y: 0, w: 0, z: 0, x: 0} 1
   {y: 0, w: 0, z: 1, x: 0} 1
   {y: 1, w: 0, z: 0, x: 0} 1
   {y: 1, w: 0, z: 1, x: 0} 1
   {y: 0, w: 0, z: 0, x: 1} 1
   {y: 0, w: 0, z: 1, x: 1} 1
   {y: 1, w: 0, z: 0, x: 1} 1
   {y: 1, w: 0, z: 1, x: 1} 1
   {y: 0, w: 1, z: 0, x: 0} 0
   {y: 0, w: 1, z: 1, x: 0} 1
   {y: 1, w: 1, z: 0, x: 0} 0
   {y: 1, w: 1, z: 1, x: 0} 1
   {y: 0, w: 1, z: 0, x: 1} 1
   {y: 0, w: 1, z: 1, x: 1} 0
   {y: 1, w: 1, z: 0, x: 1} 0
   {y: 1, w: 1, z: 1, x: 1} 1
