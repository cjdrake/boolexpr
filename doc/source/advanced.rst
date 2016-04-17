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
   >>> xs = [ctx.get_var("x_%s" % i) for i in range(8)]

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

Boolean algebra has something called the *duality principle*.
The most common demonstration of that principle is DeMorgan's Law:

.. code-block:: pycon

   >>> nor(a, b).pushdown_not()
   And(~a, ~b)
   >>> nand(a, b).pushdown_not()
   Or(~a, ~b)

DeMorgan's Law demonstrates that OR is the *dual* operator of AND,
and vice-versa.
In fact,
all the BoolExpr operators except for ``Implies`` have a *dual* operator.

.. code-block:: pycon

   >>> xnor(a, b).pushdown_not()
   Xor(~a, b)
   >>> neq(a, b).pushdown_not()
   Equal(~a, b)
   >>> (~impl(p, q)).pushdown_not()
   And(p, ~q)
   >>> (~ite(s, d1, d0)).pushdown_not()
   IfThenElse(s, ~d1, ~d0)

Using these identities recursively,
you can push all negated nodes in the expression graph down towards the leaves,
where they will change 0 for 1, and :math:`\overline{x}` to :math:`x`.

For example:

.. code-block:: pycon

   >>> f = nor(a&b, c^d)
   >>> f.pushdown_not()
   And(Or(~a, ~b), Xor(~c, d))

As you can see,
the NOT operator at the top of the expression is pushed all the way down to
the literals at the leaves.

The value of this transformation is that it eliminates all negative operators
from the expression graph.

Convert N-ary Ops to Binary Ops
-------------------------------

If, for some reason,
you want to convert N-ary expressions to binary forms,
use the ``to_binop`` method:

.. code-block:: pycon

   >>> or_(*xs[:8]).to_binop()
   Or(Or(Or(x_0, x_1), Or(x_2, x_3)), Or(Or(x_4, x_5), Or(x_6, x_7)))

Convert All Operators to OR/AND Form
------------------------------------

The most common basis for Boolean algebra is NOT/OR/AND.
The ``to_latop`` transformation converts all ``Xor``, ``Equal``, ``Implies``,
and ``IfThenElse`` operators,
and converts them to their most obvious form using NOT/OR/AND.

For example:

.. code-block:: pycon

   >>> xor(a, b).to_latop()
   Or(And(~a, b), And(a, ~b))
   >>> eq(a, b).to_latop()
   Or(And(~a, ~b), And(a, b))
   >>> impl(p, q).to_latop()
   Or(~p, q)
   >>> ite(s, d1, d0).to_latop()
   Or(And(s, d1), And(~s, d0))

The two-level conversion from XOR to OR/AND is exponential in size,
so ``to_latop`` chooses to return a smaller, but deeper form:

.. code-block:: pycon

   >>> xor(a, b, c, d).to_latop()
   Or(And(Nor(And(~a, b), And(a, ~b)), Or(And(~c, d), And(c, ~d))), And(Or(And(~a, b), And(a, ~b)), Nor(And(~c, d), And(c, ~d))))

Negation Normal Form
--------------------

A Boolean expression is in
`negation normal form (NNF) <https://en.wikipedia.org/wiki/Negation_normal_form>`_
if it contains only
literals, and OR/AND operators.
This is the same as converting to lattice operator (``to_latop``),
then pushing down all NOT operators towards the leaves (``pushdown_not``).
Use the ``to_nnf`` method to combine these transformations.

.. code-block:: pycon

   >>> f = xor(eq(a, b), impl(p, q), ite(s, d1, d0))
   >>> f.to_nnf()
   Or(And(Or(And(Or(~d0, s), Or(~d1, ~s)), ~p, q), Or(And(d1, s), And(d0, ~s), And(~q, p)), Or(And(b, a), And(~b, ~a))), And(Or(And(Or(~d1, ~s), Or(~d0, s), Or(q, ~p)), And(Or(And(d0, ~s), And(d1, s)), p, ~q)), Or(b, a), Or(~b, ~a)))

Conjunctive/Disjunctive Normal Form
-----------------------------------

The conjunctive (CNF), and disjunctive (DNF) normal forms are NNF expressions
with a depth less than or equal to two.
CNF is a conjunctive (AND) of clauses,
and DNF is a disjunction (OR) of clauses.

To convert expressions to CNF and DNF,
use the ``to_cnf``, and ``to_dnf`` methods, respectively.

The process of flattening an expression to two-level form causes an exponential
blow-up of the graph size.
Use these methods with caution.

For example:

.. code-block:: pycon

   >>> f = xor(eq(a, b), impl(p, q), ite(s, d1, d0))
   >>> f.to_cnf()
   And(Or(~q, ~s, ~d1, ~b, a), Or(~q, s, b, ~p, ~d0, ~a), Or(~q, ~d1, b, ~d0, ~a), Or(~q, ~s, ~d1, b, ~p, ~a),
       Or(~q, ~d1, b, ~p, ~d0, ~a), Or(p, s, ~b, a, ~d0), Or(p, ~s, ~d1, ~b, a), Or(~s, ~d1, ~b, a, p, d0),
       Or(p, ~d1, ~b, a, ~d0), Or(p, s, b, ~d0, ~a), Or(p, ~s, ~d1, b, ~a), Or(~s, d1, ~b, q, a, ~p),
       Or(q, d1, b, ~p, d0, ~a), Or(~q, ~s, d1, b, a), Or(~q, ~s, d1, b, a, ~p), Or(b, a, ~q, s, d0),
       Or(~q, s, ~b, a, ~d0), Or(d1, b, a, ~p, ~q, d0), Or(s, d1, b, q, a, ~p, ~d0), Or(d1, b, a, p, d0),
       Or(~d1, b, a, p, s, d0), Or(p, ~d1, b, ~d0, ~a), Or(~s, ~d1, b, q, a, ~p), Or(q, ~d1, ~b, ~p, ~d0, ~a),
       Or(~d1, b, a, ~q, s, d0), Or(~q, s, d1, ~b, a, ~d0), Or(~q, ~s, ~d1, ~b, a, ~p), Or(~s, ~d1, b, ~q, d0, ~a),
       Or(d1, ~b, ~p, ~q, d0, ~a), Or(d1, b, a, ~q, d0), Or(d1, ~b, p, d0, ~a), Or(~q, ~d1, ~b, a, ~p, ~d0),
       Or(q, b, ~p, s, d0, ~a), Or(~q, ~d1, ~b, a, ~d0), Or(~b, ~q, s, d0, ~a), Or(~b, p, s, d0, ~a),
       Or(~d1, b, q, a, ~p, ~d0), Or(b, a, p, s, d0), Or(~d1, ~b, ~p, ~q, s, d0, ~a), Or(~d1, ~b, ~q, s, d0, ~a),
       Or(~s, ~d1, ~b, a, ~q, d0), Or(~s, ~d1, b, p, d0, ~a), Or(~q, ~s, d1, ~b, ~p, ~a), Or(d1, ~b, ~q, d0, ~a),
       Or(b, a, ~p, ~q, s, d0), Or(~d1, b, a, ~p, ~q, s, d0), Or(~q, s, b, ~d0, ~a), Or(q, s, ~b, ~p, ~d0, ~a),
       Or(d1, ~b, q, a, ~p, d0), Or(~d1, ~b, q, a, ~p, s, d0), Or(~b, q, a, ~p, s, d0), Or(s, b, q, a, ~p, ~d0),
       Or(p, ~s, d1, b, a, ~d0), Or(~q, s, ~b, a, ~p, ~d0), Or(q, ~d1, b, ~p, s, d0, ~a), Or(~b, ~p, ~q, s, d0, ~a),
       Or(~q, ~s, d1, b, a, ~d0), Or(~q, ~s, d1, ~b, ~d0, ~a), Or(p, s, d1, ~b, a, ~d0), Or(~q, ~s, ~d1, b, ~a),
       Or(p, ~s, d1, ~b, ~d0, ~a), Or(p, ~s, d1, b, a), Or(p, ~s, d1, ~b, ~a), Or(~q, s, d1, b, ~p, ~d0, ~a),
       Or(~q, ~s, d1, ~b, ~a), Or(p, s, d1, b, ~d0, ~a), Or(q, ~s, d1, b, ~p, ~a), Or(q, s, d1, ~b, ~p, ~d0, ~a),
       Or(q, ~s, ~d1, ~b, ~p, ~a), Or(~d1, ~b, p, s, d0, ~a), Or(~q, s, d1, b, ~d0, ~a), Or(~q, s, d1, ~b, a, ~p, ~d0))

Tseytin Transformation
----------------------

SAT solvers such as CryptoMiniSAT require a CNF input.
Since the ``to_cnf`` method might require exponential memory,
we need another way to transform an arbitrary expression to a CNF that is
*equisatisfiable* with the original.

The answer is the
`Tseytin transformation <https://en.wikipedia.org/wiki/Tseytin_transformation>`_.
Since this transformation creates auxiliary variables,
you must provide a ``Context`` object instance to manage those new variables.

Use the ``tseytin`` method to get the Tseytin transformation.
Notice how in the following example,
the Tseytin form is much smaller than the aforementioned CNF form.

.. code-block:: pycon

   >>> f = xor(eq(a, b), impl(p, q), ite(s, d1, d0))
   >>> f.tseytin(ctx)
   And(Or(b, a, a_1), Or(~b, ~a, a_1), Or(a_2, ~a_3, ~a_1, ~a_0), Or(b, ~a_1, ~a),
       Or(a_3, ~a_2, ~a_1, ~a_0), Or(~a_3, a_2, a_1, a_0), Or(~a_2, ~a_3, ~a_1, a_0), a_0, Or(~d0, a_3, s),
       Or(~a_3, d0, s), Or(d0, ~a_3, d1), Or(d1, ~a_3, ~s), Or(q, ~a_2, ~p),
       Or(~b, ~a_1, a), Or(~q, a_2), Or(a_3, ~a_2, a_1, a_0), Or(a_3, a_2, ~a_1, a_0),
       Or(a_3, a_2, a_1, ~a_0), Or(~d1, a_3, ~s), Or(~a_3, ~a_2, a_1, ~a_0), Or(p, a_2))

Variable Substitution
=====================

Satisfiability
==============

Cofactors
=========
