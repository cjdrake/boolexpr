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
   >>> xs = array('x', 8)

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
All ``BoolExpr`` objects support simplification using the
:meth:`simplify <boolexpr.BoolExpr.simplify>` method.
It is probably more convenient to use the auto-simplifying function forms,
e.g. :meth:`or_s <boolexpr.or_s>` instead of :meth:`or_ <boolexpr.or_>`.

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

The :class:`Logical <boolexpr.Logical>` expression node represents a value
that is constant,
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

BoolExpr knows about several identities involving the
:class:`Implies <boolexpr.Implies>` and
:class:`IfThenElse <boolexpr.IfThenElse>`
operators as well.

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

The :class:`IfThenElse <boolexpr.IfThenElse>` operator is the same as a
2:1 multiplexer.
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

   >>> nor(a, b).to_posop()
   And(~a, ~b)
   >>> nand(a, b).to_posop()
   Or(~a, ~b)

DeMorgan's Law demonstrates that OR is the *dual* operator of AND,
and vice-versa.
In fact,
all the BoolExpr operators have a *dual* operator.

.. code-block:: pycon

   >>> xnor(a, b).to_posop()
   Xor(~a, b)
   >>> neq(a, b).to_posop()
   Equal(~a, b)
   >>> (~impl(p, q)).to_posop()
   And(p, ~q)
   >>> (~ite(s, d1, d0)).to_posop()
   IfThenElse(s, ~d1, ~d0)

Using these identities recursively,
you can push all negated nodes in the expression graph down towards the leaves,
where they will change 0 for 1, and :math:`\overline{x}` to :math:`x`.
This leaves you with all positive operators in the expression.

For example:

.. code-block:: pycon

   >>> f = nor(a&b, c^d)
   >>> f.to_posop()
   And(Or(~a, ~b), Xor(~c, d))

As you can see,
the NOT operator at the top of the expression is pushed all the way down to
the literals at the leaves.

Convert N-ary Ops to Binary Ops
-------------------------------

If, for some reason,
you want to convert N-ary expressions to binary forms,
use the :meth:`to_binop <boolexpr.BoolExpr.to_binop>` method:

.. code-block:: pycon

   >>> or_(*xs[:8]).to_binop()
   Or(Or(Or(x_0, x_1), Or(x_2, x_3)), Or(Or(x_4, x_5), Or(x_6, x_7)))

This technique might help to re-write an arbitrary expression in a language
(such as Python REPL) that only supports binary operators.

Convert All Operators to OR/AND Form
------------------------------------

The most common basis for Boolean algebra is NOT/OR/AND.
The :meth:`to_latop <boolexpr.BoolExpr.to_latop>` transformation converts all
:class:`Xor <boolexpr.Xor>`,
:class:`Equal <boolexpr.Equal>`,
:class:`Implies <boolexpr.Implies>`,
and :class:`IfThenElse <boolexpr.IfThenElse>` operators to their most obvious
form using NOT/OR/AND.

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
so :meth:`to_latop <boolexpr.BoolExpr.to_latop>` chooses to return a smaller,
nested form:

.. code-block:: pycon

   >>> xor(a, b, c, d).to_latop()
   Or(And(Nor(And(~a, b), And(a, ~b)), Or(And(~c, d), And(c, ~d))), And(Or(And(~a, b), And(a, ~b)), Nor(And(~c, d), And(c, ~d))))

Negation Normal Form
--------------------

A Boolean expression is in
`negation normal form (NNF) <https://en.wikipedia.org/wiki/Negation_normal_form>`_
if it contains only
literals, and OR/AND operators.
This is the same as converting to lattice operator,
then pushing down all NOT operators towards the leaves.
Use the
:meth:`to_nnf <boolexpr.BoolExpr.to_nnf>` method
to combine these transformations.

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
use the :meth:`to_cnf <boolexpr.BoolExpr.to_cnf>`,
and :meth:`to_dnf <boolexpr.BoolExpr.to_dnf>` methods,
respectively.

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
Since the :meth:`to_cnf <boolexpr.BoolExpr.to_cnf>`
method might require exponential memory,
we need another way to transform an arbitrary expression to a CNF that is
*equisatisfiable* with the original.

The answer is the
`Tseytin transformation <https://en.wikipedia.org/wiki/Tseytin_transformation>`_.
Since this transformation creates auxiliary variables,
you must provide a :class:`Context <boolexpr.Context>`
object instance to manage those new variables.

Use the :meth:`tseytin <boolexpr.BoolExpr.tseytin>`
method to get the Tseytin transformation.
Notice how in the following example,
the Tseytin form is much smaller than its aforementioned CNF form.

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

Function composition on Boolean expressions is fairly straightforward.
Simply substitute some subset of support variables with other expressions.
For this,
BoolExpr provides the :meth:`compose <boolexpr.BoolExpr.compose>` method.
It takes a ``dict`` of ``{Variable: BoolExpr}`` mappings as input,
and *does not* auto-simplify the output.

For example:

.. code-block:: pycon

   >>> f = a | b & c ^ d
   >>> g = impl(p, q)
   >>> f.compose({a: g})
   Or(Implies(p, q), Xor(And(b, c), d))
   >>> f.compose({d: g})
   Or(a, Xor(And(b, c), Implies(p, q)))

The :meth:`restrict <boolexpr.BoolExpr.restrict>` method
is similar to :meth:`compose <boolexpr.BoolExpr.compose>`.
It takes a ``dict`` of ``{Variable: Constant}`` mappings as input,
and *does* auto-simpify the output.

For example:

.. code-block:: pycon

   >>> f = a | b & c ^ d
   >>> f.restrict({a: 0})
   Xor(d, And(c, b))
   >>> f.restrict({a: False, b: True, c: False, d: True})
   1
   >>> f.restrict({c: 'X'})
   X

Satisfiability
==============

The question of whether a Boolean function is *satisfiable* (SAT) is one of the
most important questions in computer science.
To help us answer this question,
BoolExpr has the :meth:`sat <boolexpr.BoolExpr.sat>` and
:meth:`iter_sat <boolexpr.BoolExpr.iter_sat>` methods.
SAT is NP-complete,
so it is not guaranteed that a solution can be found quickly.
Under the hood,
BoolExpr uses the modern, industrial-strength
`CryptoMiniSAT <https://github.com/msoos/cryptominisat>`_
solver to arrive at solutions as quickly as possible.

The :meth:`sat <boolexpr.BoolExpr.sat>` method returns a two-tuple.
The first part is the ``bool`` answer to whether the function is satisfiable.
If the function is SAT,
the second part will contain a satisfying input point.

For example:

.. code-block:: pycon

   >>> f = (~a|~b) & (~a|b) & (a|~b) & (a|b)
   >>> f.sat()
   (False, None)
   >>> g = xor(eq(a, b), impl(p, q), ite(s, d1, d0))
   >>> g.sat()
   (True, {d1: 1, d0: 1, q: 1, a: 1, b: 1, s: 1, p: 1})

The :meth:`iter_sat <boolexpr.BoolExpr.iter_sat>` method is a generator
that iterates through all satisfying input points.
Unsatisfiable functions will be empty.

For example:

.. code-block:: pycon

   >>> f = (~a|~b) & (~a|b) & (a|~b) & (a|b)
   >>> list(f.iter_sat())
   []
   >>> g = onehot(a, b, c)
   >>> list(g.iter_sat())
   [{b: 1, c: 0, a: 0}, {b: 0, c: 0, a: 1}, {b: 0, c: 1, a: 0}]

Cofactors
=========

The Shannon expansion is the fundamental theorem of Boolean algebra.
To make it easier to calculate this,
BoolExpr provides the :meth:`iter_cfs <boolexpr.BoolExpr.iter_cfs>`
generator method.

You can use it with only one input variable, the common case:

.. code-block:: pycon

   >>> list(ite(s, d1, d0).iter_cfs(s))
   [d0, d1]

Or you can view the cofactors of multiple variables simultaneously:

.. code-block:: pycon

   >>> list(ite(s, d1, d0).iter_cfs([d1, d0]))
   [0, s, ~s, 1]

Existential and Universal Quantification
----------------------------------------

Some logical statements are structured such that *there exists* a value of
a variable :math:`x` such that the statement is true.
This is the existential quantification operator.
BoolExpr provides the :meth:`smoothing <boolexpr.BoolExpr.smoothing>`
method for this.
The smoothing is the OR of a sequence of cofactors.

For convenience, you can also use the :func:`exists <boolexpr.exists>` function.

For example,
for a function :math:`f` that depends on :math:`a`,
to write "there exists a variable :math:`a` such that :math:`f` is true":

.. code-block:: pycon

   >>> f = onehot0(a, b, c)
   >>> f.smoothing(a)
   Or(And(Or(~c, ~b), ~c, ~b), ~b, ~c)
   >>> exists(a, f)
   Or(And(Or(~c, ~b), ~c, ~b), ~b, ~c)

Similarly, you can write logical statements structured such that *for all*
values of a variable :math:`x` such that the statement is true.
This is the universal quantification operator.
BoolExpr provides the :meth:`consensus <boolexpr.BoolExpr.consensus>`
method for this.
The consensus is the AND of a sequence of cofactors.

For convenience, you can also use the :func:`forall <boolexpr.forall>` function.

For example,
for a function :math:`f` that depends on :math:`a`,
to write "for all values of :math`a`, :math:`f` is true":

.. code-block:: pycon

   >>> f = onehot0(a, b, c)
   >>> f.consensus(a)
   And(~c, ~b, Or(~c, ~b), Or(~c, ~b))
   >>> forall(a, f)
   And(~c, ~b, Or(~c, ~b), Or(~c, ~b))

The :meth:`derivative <boolexpr.BoolExpr.derivative>` method is similar to
:meth:`smoothing <boolexpr.BoolExpr.smoothing>` and
:meth:`consensus <boolexpr.BoolExpr.consensus>`.
It is the XOR of a sequence of cofactors.

.. code-block:: pycon

   >>> f = onehot0(a, b, c)
   >>> f.derivative(a)
   Xor(And(Or(~c, ~b), ~c, ~b), Or(~c, ~b))

Multi-Dimensional Arrays
========================

BoolExpr version 2.0 introduces multi-dimensional arrays.
The look and feel of the ``boolexpr.ndarray`` data type is modeled after
numpy's `ndarray <http://docs.scipy.org/doc/numpy/reference/generated/numpy.ndarray.html>`_,
with some subtle differences.

Construction
------------

There are four ways to construct ``ndarray`` instances:

1. Use the ``Context.get_vars`` method.
2. Use the ``array`` factory function.
3. Use the ``zeros``, ``ones``, and ``logicals`` factory functions.
4. Use the ``uint2nda``, and ``int2nda`` factory functions.

We have already covered #1.

The ``array`` function takes an arbitrarily-shaped input of Python sequences,
and attempts to automatically convert it to an ``ndarray``.
For example:

.. code-block:: pycon

   >>> a, b, c, d = map(ctx.get_var, "abcd")
   >>> array([a, b, c, d])
   array([a, b, c, d])
   >>> array([[a, b], [c, d]])
   array([[a, b],
          [c, d]])

If you want to get an N-dimensional array of constant values,
use ``zeros``, ``ones``, and ``logicals``:

.. code-block:: pycon

   >>> zeros(4)
   array([0, 0, 0, 0])
   >>> ones(2, 3)
   array([[1, 1, 1],
          [1, 1, 1]])

Similarly, to get the binary representation of an unsigned or two's complement
integer, use the ``uint2nda`` and ``int2nda`` functions.
Both take an optional ``length`` parameter.

.. code-block:: pycon

   >>> uint2nda(42)
   array([0, 1, 0, 1, 0, 1])
   >>> int2nda(-42, width=8)
   array([0, 1, 1, 0, 1, 0, 1, 1])

Notice that for integers,
the output is displayed in least-significant-bit to most-significant-bit order.

Shape and Size
--------------

The ``Context.get_vars`` method allows you to provide irregular shape inputs.
For example:

.. code-block:: pycon

   >>> X = ctx.get_vars('x', (1, 3), (2, 4), (3, 5))
   >>> X
   array([[[x[1,2,3], x[1,2,4]],
           [x[1,3,3], x[1,3,4]]],

          [[x[2,2,3], x[2,2,4]],
           [x[2,3,3], x[2,3,4]]]])

You can accomplish the same thing with the ``array`` function using the
optional ``shape`` parameter:

.. code-block:: pycon

   >>> Y = array([[a, b], [c, d]], shape=((5, 7), (11, 13)))
   >>> Y
   array([[a, b],
          [c, d]])

Internally, all of these are represented as flat arrays.

Use the ``shape`` property to get the array shape,
and the ``reshape`` method to return a new array with a similar shape:

.. code-block:: pycon

   >>> Y.shape
   ((5, 7), (11, 13))
   >>> Y.reshape(2, 2).shape
   ((0, 2), (0, 2))

Keep in mind that iterating through an N-dimensional array will only work on
one dimension at a time.
Therefore, the ``len`` function will only return the length of the first
dimension in that iteration:

.. code-block:: pycon

   >>> for item in X:
           print(item)
   array([[x[1,2,3], x[1,2,4]],
          [x[1,3,3], x[1,3,4]]])
   array([[x[2,2,3], x[2,2,4]],
          [x[2,3,3], x[2,3,4]]])
   >>> len(X)
   2

Use the ``flat`` iterator and ``size`` property to access the array data in a
one-dimensional way:

.. code-block:: pycon

   >>> for item in X.flat:
           print(item)
   x[1,2,3]
   x[1,2,4]
   x[1,3,3]
   x[1,3,4]
   x[2,2,3]
   x[2,2,4]
   x[2,3,3]
   x[2,3,4]
   >>> X.size
   8

Slicing
-------

N-dimensional arrays mostly support numpy-style slicing.

To demonstrate the various capabilities, let's create some arrays.
For simplicity, we will only use zero indexing.

.. code-block:: pycon

   >>> A = ctx.get_vars('a', 4)
   >>> B = ctx.get_vars('b', 4, 4, 4)

Using a single integer index will *collapse* an array dimension.
For 1-D arrays,
this means returning an item.

.. code-block:: pycon

   >>> A[2]
   a[2]
   >>> B[2]
   array([[b[2,0,0], b[2,0,1], b[2,0,2], b[2,0,3]],
          [b[2,1,0], b[2,1,1], b[2,1,2], b[2,1,3]],
          [b[2,2,0], b[2,2,1], b[2,2,2], b[2,2,3]],
          [b[2,3,0], b[2,3,1], b[2,3,2], b[2,3,3]]])

The colon ``:`` slice syntax *shrinks* a dimension:

.. code-block:: pycon

   >>> A[:]
   array([a[0], a[1], a[2], a[3]])
   >>> A[1:]
   array([a[1], a[2], a[3]])
   >>> A[:3]
   array([a[0], a[1], a[2]])
   >>> B[1:3]
   array([[[b[1,0,0], b[1,0,1], b[1,0,2], b[1,0,3]],
           [b[1,1,0], b[1,1,1], b[1,1,2], b[1,1,3]],
           [b[1,2,0], b[1,2,1], b[1,2,2], b[1,2,3]],
           [b[1,3,0], b[1,3,1], b[1,3,2], b[1,3,3]]],

          [[b[2,0,0], b[2,0,1], b[2,0,2], b[2,0,3]],
           [b[2,1,0], b[2,1,1], b[2,1,2], b[2,1,3]],
           [b[2,2,0], b[2,2,1], b[2,2,2], b[2,2,3]],
           [b[2,3,0], b[2,3,1], b[2,3,2], b[2,3,3]]]])

For N-dimensional arrays,
the slice accepts up to N indices separated by a comma.
Unspecified slices at the end will default to ``:``.

.. code-block:: pycon

   >>> B[1,2,3]
   b[1,2,3]
   >>> B[:,2,3]
   array([b[0,2,3], b[1,2,3], b[2,2,3], b[3,2,3]])
   >>> B[1,:,3]
   array([b[1,0,3], b[1,1,3], b[1,2,3], b[1,3,3]])
   >>> B[1,2,:]
   array([b[1,2,0], b[1,2,1], b[1,2,2], b[1,2,3]])
   >>> B[1,2]
   array([b[1,2,0], b[1,2,1], b[1,2,2], b[1,2,3]])

The ``...`` syntax will fill available indices left to right with ``:``.
Only one ellipsis will be recognized per slice.

.. code-block:: pycon

   >>> B[...,1]
   array([[b[0,0,1], b[0,1,1], b[0,2,1], b[0,3,1]],
          [b[1,0,1], b[1,1,1], b[1,2,1], b[1,3,1]],
          [b[2,0,1], b[2,1,1], b[2,2,1], b[2,3,1]],
          [b[3,0,1], b[3,1,1], b[3,2,1], b[3,3,1]]])
   >>> B[1,...]
   array([[b[1,0,0], b[1,0,1], b[1,0,2], b[1,0,3]],
          [b[1,1,0], b[1,1,1], b[1,1,2], b[1,1,3]],
          [b[1,2,0], b[1,2,1], b[1,2,2], b[1,2,3]],
          [b[1,3,0], b[1,3,1], b[1,3,2], b[1,3,3]]])

N-dimensional arrays also support negative indices.
Arrays with a zero start index follow Python's usual conventions.

For example, here is the index guide for ``A[0:4]``::

    +------+------+------+------+
    | a[0] | a[1] | a[2] | a[3] |
    +------+------+------+------+
    0      1      2      3      4
   -4     -3     -2     -1

For example

.. code-block:: pycon

   >>> A[-1]
   a[3]
   >>> A[-3:-1]
   array([a[1], a[2]])

N-dimensional arrays are mutable,
which means you can use the slicing notation similarly to a Python list:

.. code-block:: pycon

   >>> A
   array([a[0], a[1], a[2], a[3]])
   >>> A[1] = B[1,2,3]
   >>> A
   array([a[0], b[1,2,3], a[2], a[3]])

If you assign to a slice larger than one element,
make sure the size of the left-hand slice matches
the size of the right-hand slice.


.. code-block:: pycon

   >>> A[1:3] = B[1,1:3,1]
   >>> A
   array([a[0], b[1,1,1], b[1,2,1], a[3]])

Operators
---------

Bitwise
^^^^^^^

N-dimensional arrays overload all of Python's bit-wise operators.

.. code-block:: pycon

   >>> X = ctx.get_vars('x', 4)
   >>> Y = ctx.get_vars('y', 4)
   >>> ~X
   array([~x[0], ~x[1], ~x[2], ~x[3]])
   >>> X | Y
   array([Or(x[0], y[0]), Or(x[1], y[1]), Or(x[2], y[2]), Or(x[3], y[3])])
   >>> X & Y
   array([And(x[0], y[0]), And(x[1], y[1]), And(x[2], y[2]), And(x[3], y[3])])
   >>> X ^ Y
   array([Xor(x[0], y[0]), Xor(x[1], y[1]), Xor(x[2], y[2]), Xor(x[3], y[3])])
   >>> X << 2
   array([0, 0, x[0], x[1]])
   >>> X >> 2
   array([x[2], x[3], 0, 0])

Performing bitwise operators on arrays of different size works just fine,
but keep in mind that it fills empty elements with the operator's *identity*.
For OR and XOR, this is ``False``; for AND, this is ``True``.

.. code-block:: pycon

   >>> X & Y[:2]
   array([And(x[0], y[0]), And(x[1], y[1]), x[2], x[3]])

Concatenation and Repetition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Just like Python lists, the ``+`` operator implements concatenation,
and the ``*`` operator implements repetition.

.. code-block:: pycon

   >>> X + Y
   array([x[0], x[1], x[2], x[3], y[0], y[1], y[2], y[3]])
   >>> Y + X
   array([y[0], y[1], y[2], y[3], x[0], x[1], x[2], x[3]])
   >>> X * 2
   array([x[0], x[1], x[2], x[3], x[0], x[1], x[2], x[3]])
   >>> 2 * X
   array([x[0], x[1], x[2], x[3], x[0], x[1], x[2], x[3]])

Reduction
^^^^^^^^^

Another important operator for bit vectors is *reduction*.
You can implement reduction the usual Python way with the
``operator`` and ``functools`` modules.

.. code-block:: pycon

   >>> from functools import reduce
   >>> import operator
   >>> reduce(operator.or_, X)
   Or(Or(Or(x[0], x[1]), x[2]), x[3])

You can simplify this output to get the desired result:

.. code-block:: pycon

   >>> reduce(operator.or_, X).simplify()
   Or(x[1], x[0], x[3], x[2])

To make things a bit easier, ``boolexpr`` provides bitwise reduction operators.

.. code-block:: pycon

   >>> X.or_reduce()
   Or(x[0], x[1], x[2], x[3])
   >>> X.and_reduce()
   And(x[0], x[1], x[2], x[3])
   >>> X.xor_reduce()
   Xor(x[0], x[1], x[2], x[3])

Extension
^^^^^^^^^

The ``zext`` method returns a new array *zero-extended* by some number of bits.
Similarly,
the ``sext`` method returns a new array *sign-extended* by some number of bits.
Even though N-dimensional arrays do not posess any numerical encoding semantics,
the meaning is the same for either signed magnitude or two's complement,
where the sign is the most-significant bit.
Notice that the return value is a new array.

For example:

.. code-block:: pycon

   >>> X.zext(2)
   array([x[0], x[1], x[2], x[3], 0, 0])
   >>> X.sext(2)
   array([x[0], x[1], x[2], x[3], x[3], x[3]])

Rich Shift
^^^^^^^^^^

The shift operators we have looked at so far are somewhat limited.
The ``lsh`` and ``rsh`` shift methods give you not just the array
after being shifted,
but also the value that is "shifted out" of the array.
Instead of an integer, they take an array as an input.

For example:

.. code-block:: pycon

   >>> X.lsh(Y[:2])
   (array([y[0], y[1], x[0], x[1]]), array([x[2], x[3]]))
   >>> X.rsh(ones(2))
   (array([x[0], x[1]]), array([x[2], x[3], 1, 1]))

Notice that the return value maintains the direction of the shift-out.
That is,
for left shifts the value shifts towards the most significant bit,
and for right shifts, the value shifts towards the least significant bit.

For convenience, there is also an "arithmetic" shift operator.
This is a type of right shift that always uses the most significant bit
as the shift-in value.
This preserves the signedness in a signed magnitude or two's complement
representation.

For example:

.. code-block:: pycon

   >>> X.arsh(2)
   (array([x[0], x[1]]), array([x[2], x[3], x[3], x[3]]))

Other Methods
--------------

N-dimensional arrays also implement some vectorized versions of BoolExpr
methods.

For example,
you can perform function composition or restriction on a whole array at once:

.. code-block:: pycon

   >>> X = array(x, 4)
   >>> X.compose({X[0]: Y[0] | Y[1], X[1]: Y[2] & Y[3]})
   array([Or(y[0], y[1]), And(y[2], y[3]), x[2], x[3]])
   >>> X.restrict({X[0]: 0, X[1]: 1, X[2]: 0, X[3]: 1})
   array([0, 1, 0, 1])
   >>> X.restrict({X: "0101"})
   array([0, 1, 0, 1])

The ``equiv`` method tests whether two N-dimensional arrays are equivalent.
This is only true if both arrays are the same size,
and each corresponding element of both arrays is equivalent.

.. code-block:: pycon

   >>> A = array([impl(p, q), ite(s, d1, d0)])
   >>> B = array([~p|q, s&d1|~s&d0])
   >>> A.equiv(B)
   True
