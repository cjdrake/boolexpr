.. Copyright 2016 Chris Drake

.. _advanced:

************
  Advanced
************

This chapter is a tour of advanced features.
By "advanced",
we just mean the explanations are a bit more involved than basic Boolean
function input and output.

The code blocks in this chapter assume you have executed the following in
a terminal:

.. code-block:: pycon

   >>> from boolexpr as bx
   >>> ctx = bx.Context()
   >>> a, b, c, d = map(ctx.get_var, "abcd")
   >>> p, q, s, d1, d0 = map(ctx.get_var, "p q s d1 d0".split())

Trivial Operators
=================

All of the N-ary operators have Python variadic functions.
Giving zero or one argument to those functions results in a degenerate form.

.. code-block:: pycon

   >>> bx.nor()
   1
   >>> bx.nor(a)
   ~a
   >>> bx.or_()
   0
   >>> bx.or_(a)
   a
   >>> bx.nand()
   0
   >>> bx.nand(a)
   ~a
   >>> bx.and_()
   1
   >>> bx.and_(a)
   a
   >>> bx.xnor()
   1
   >>> bx.xnor(a)
   ~a
   >>> bx.xor()
   0
   >>> bx.xor(a)
   a
   >>> bx.neq()
   0
   >>> bx.neq(a)
   0
   >>> bx.eq()
   1
   >>> bx.eq(a)
   1

Transformations
===============

Boolean expressions are not a canonical form.
That means you can apply various transformations to them,
and end up with different,
functionally equivalent forms with different properties.
Sometimes, the new form is more useful for a particular application.

Simplification
--------------

There are several Boolean identities that reduce the size of an expression,
and are very cheap to apply.
All ``BoolExpr`` objects support simplification using the ``simplify`` method.
It is probably more convenient to use the auto-simplifying function forms,
e.g. ``or_s`` instead of ``or_``.

Eliminating Constants
^^^^^^^^^^^^^^^^^^^^^

Simplification eliminates all constant values from expressions.
For example:

.. code-block:: pycon

   >>> bx.or_s(a, False)
   a
   >>> bx.xor_s(a|1, b)
   ~b

This is also true for expressions that can easily be converted to constants.
For example:

.. code-block:: pycon

   >>> bx.or_s(a, ~a)
   1

Associativity
^^^^^^^^^^^^^

The OR, AND, and XOR operators are associative.
In cases where an associative operator finds sub-operators of the same type,
the operators will be collapsed.
For example:

.. code-block:: pycon

   >>> bx.or_s(a&b, c|d)
   Or(c, d, And(b, a))

Unknown Propagation
^^^^^^^^^^^^^^^^^^^

The ``Logical`` expression node represents a value that is constant,
but the particular 0/1 value is not known.
The simplification operator will perform *optimistic* X propagation on
these values.

For example:

.. code-block:: pycon

   >>> bx.or_s(a, 'X')
   X

The worth of this analysis is determining whether sub-expressions that propagate
known constants *dominate* sub-expressions that propagate unknown constants.

For example:

.. code-block:: pycon

   >>> bx.or_s(a & 0, b & 'X')
   X
   >>> bx.or_s(1 | a, b & 'X')
   1

In the first case,
the unknown value dominates the OR expression, resulting in an ``X`` output.
In the second case,
the ``1`` dominates the ``X``, resulting in a ``1`` output.

Miscellaneous
^^^^^^^^^^^^^

BoolExpr knows about several identities involving the ``implies`` and ``ite``
operator as well.

For example:

.. code-block:: pycon

   >>> bx.impl_s(0, q)
   1
   >>> bx.impl_s(~p, p)
   p
   >>> bx.ite_s(0, d1, d0)
   d0
   >>> bx.ite_s(s, d1, 1)
   Or(d1, ~s)

The ``ITE`` operator is an "if-then-else",
which is the same as a 2:1 multiplexer.
When you apply the same value to both inputs,
it doesn't matter what the select value is.

.. code-block:: pycon

   >>> bx.ite_s(s, d1, d1)
   d1
   >>> bx.ite_s('X', 0, 0)
   0
   >>> bx.ite_s('X', a, a)
   a

Push Down NOT Bubbles
---------------------

Convert N-ary Ops to Binary Ops
-------------------------------

Convert All Operators to OR/AND Form
------------------------------------

Negation Normal Form
--------------------

Conjunctive/Disjunctive Normal Form
-----------------------------------

Tseytin Transformation
----------------------

Variable Substitution
=====================

Satisfiability
==============

Cofactors
=========
