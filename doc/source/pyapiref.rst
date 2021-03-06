.. Copyright 2016 Chris Drake

.. _pyapiref:

************************
  Python API Reference
************************

.. automodule:: boolexpr

Symbolic Variable Context
=========================

.. autoclass:: boolexpr.Context
   :members: get_var, get_vars
   :member-order: bysource

Boolean Expression Class Hierarchy
==================================

.. graphviz::

   digraph bxhier {
       rankdir="BT"

       Atom -> BoolExpr
       Operator -> BoolExpr
       Constant -> Atom
       Literal -> Atom
       Known -> Constant
       Unknown -> Constant
       Zero -> Known
       One -> Known
       Logical -> Unknown
       Complement -> Literal
       Variable -> Literal
       LatticeOperator -> Operator
       Or -> LatticeOperator
       And -> LatticeOperator
       Xor -> Operator
       Equal -> Operator
       Implies -> Operator
       IfThenElse -> Operator
   }

.. note::
   For the sake of space, this diagram omits the
   ``Nor``, ``Nand``, ``Xnor``, ``Unequal``, ``NotImplies``,
   and ``NotIfThenElse`` classes.
   They are derived from ``Operator``.

Base Class
----------

.. autoclass:: boolexpr.BoolExpr
   :members: Kind,
             to_ast, from_ast,
             to_dot,
             __invert__, __or__, __and__, __xor__,
             kind,
             depth, size,
             is_cnf, is_dnf,
             simplify, to_binop, to_latop, to_posop, tseytin,
             compose, restrict,
             sat, iter_sat,
             to_cnf, to_dnf, to_nnf,
             equiv,
             support,
             degree,
             expand,
             smoothing, consensus, derivative,
             iter_dfs,
             iter_domain,
             iter_cfs
   :member-order: bysource

Atom Nodes
----------

.. autoclass:: boolexpr.Atom
   :show-inheritance: true

.. autoclass:: boolexpr.Constant
   :show-inheritance: true

.. autoclass:: boolexpr.Known
   :show-inheritance: true

.. autoclass:: boolexpr.Zero
   :show-inheritance: true

.. autoclass:: boolexpr.One
   :show-inheritance: true

.. autoclass:: boolexpr.Unknown
   :show-inheritance: true

.. autoclass:: boolexpr.Logical
   :show-inheritance: true

.. autoclass:: boolexpr.Literal
   :show-inheritance: true
   :members: __abs__
   :member-order: bysource

.. autoclass:: boolexpr.Complement
   :show-inheritance: true

.. autoclass:: boolexpr.Variable
   :show-inheritance: true

.. autoclass:: boolexpr.Operator
   :show-inheritance: true
   :members: simple,
             args,
             is_clause
   :member-order: bysource

Operator Nodes
--------------

.. autoclass:: boolexpr.NegativeOperator
   :show-inheritance: true

.. autoclass:: boolexpr.LatticeOperator
   :show-inheritance: true

.. autoclass:: boolexpr.Nor
   :show-inheritance: true

.. autoclass:: boolexpr.Nand
   :show-inheritance: true

.. autoclass:: boolexpr.Xnor
   :show-inheritance: true

.. autoclass:: boolexpr.Unequal
   :show-inheritance: true

.. autoclass:: boolexpr.NotImplies
   :show-inheritance: true

.. autoclass:: boolexpr.NotIfThenElse
   :show-inheritance: true

.. autoclass:: boolexpr.Or
   :show-inheritance: true

.. autoclass:: boolexpr.And
   :show-inheritance: true

.. autoclass:: boolexpr.Xor
   :show-inheritance: true

.. autoclass:: boolexpr.Equal
   :show-inheritance: true

.. autoclass:: boolexpr.Implies
   :show-inheritance: true

.. autoclass:: boolexpr.IfThenElse
   :show-inheritance: true

Constant Singletons
===================

.. autodata:: boolexpr.ZERO

.. autodata:: boolexpr.ONE

.. autodata:: boolexpr.LOGICAL

.. autodata:: boolexpr.ILLOGICAL

Operator Functions
==================

Fundamental Operators
---------------------

.. autofunction:: boolexpr.not_

.. autofunction:: boolexpr.nor

.. autofunction:: boolexpr.or_

.. autofunction:: boolexpr.nand

.. autofunction:: boolexpr.and_

.. autofunction:: boolexpr.xnor

.. autofunction:: boolexpr.xor

.. autofunction:: boolexpr.neq

.. autofunction:: boolexpr.eq

.. autofunction:: boolexpr.impl

.. autofunction:: boolexpr.ite

Simplifying Operators
---------------------

.. autofunction:: boolexpr.nor_s

.. autofunction:: boolexpr.or_s

.. autofunction:: boolexpr.nand_s

.. autofunction:: boolexpr.and_s

.. autofunction:: boolexpr.xnor_s

.. autofunction:: boolexpr.xor_s

.. autofunction:: boolexpr.neq_s

.. autofunction:: boolexpr.eq_s

.. autofunction:: boolexpr.impl_s

.. autofunction:: boolexpr.ite_s

High Order Operators
--------------------

.. autofunction:: boolexpr.onehot0

.. autofunction:: boolexpr.onehot

.. autofunction:: boolexpr.nhot

.. autofunction:: boolexpr.majority

.. autofunction:: boolexpr.achilles_heel

.. autofunction:: boolexpr.mux

.. autofunction:: boolexpr.exists

.. autofunction:: boolexpr.forall

Multidimensional Arrays
=======================

.. autoclass:: boolexpr.Array

.. autofunction:: boolexpr.zeros

.. autofunction:: boolexpr.ones

.. autofunction:: boolexpr.logicals

.. autofunction:: boolexpr.uint2nda

.. autofunction:: boolexpr.int2nda

.. autofunction:: boolexpr.array

.. autoclass:: boolexpr.ndarray
   :members: shape,
             reshape,
             __iter__,
             __len__,
             __getitem__,
             __invert__, __or__, __and__, __xor__,
             __lshift__, __rshift__,
             __add__, __mul__,
             ndim, size, flat,
             simplify,
             compose, restrict,
             equiv,
             zext, sext,
             nor_reduce, or_reduce,
             nand_reduce, and_reduce,
             xnor_reduce, xor_reduce,
             lsh, rsh, arsh
   :member-order: bysource
