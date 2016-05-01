# Copyright 2016 Chris Drake
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


"""
Boolean Expression Wrapper Module

Use CFFI to interface the C++ library to Python.

Data Types:

abstract syntax tree
   A nested tuple of entries that represents an expression.
   Unlike a ``BoolExpr`` object, an ast object is serializable.

   It is defined recursively::

      ast := (BoolExpr.Kind.zero, )
           | (BoolExpr.Kind.one, )
           | (BoolExpr.Kind.log, )
           | (BoolExpr.Kind.comp, ctx, name)
           | (BoolExpr.Kind.var, ctx, name)
           | (BoolExpr.Kind.nor, ast, ...)
           | (BoolExpr.Kind.or_, ast, ...)
           | (BoolExpr.Kind.nand, ast, ...)
           | (BoolExpr.Kind.and_, ast, ...)
           | (BoolExpr.Kind.xnor, ast, ...)
           | (BoolExpr.Kind.xor, ast, ...)
           | (BoolExpr.Kind.neq, ast, ...)
           | (BoolExpr.Kind.eq, ast, ...)
           | (BoolExpr.Kind.nimpl, ast, ast)
           | (BoolExpr.Kind.impl, ast, ast)
           | (BoolExpr.Kind.nite, ast, ast, ast)
           | (BoolExpr.Kind.ite, ast, ast, ast)

      ctx := int

      name := str

   The ``ctx`` int is a pointer to a C++ Context object.
   It must be re-cast to ``void *`` before being used.

point
   A dictionary of ``{Variable : Constant}`` mappings.
   For example, ``{a: False, b: True, c: 0, d: 'X'}``.
"""


from _boolexpr import ffi, lib


__all__ = [
    "Context",
    "BoolExpr", "Atom", "Constant", "Known", "Zero", "One", "Unknown",
    "Logical", "Illogical", "Literal", "Complement", "Variable",
    "Operator", "LatticeOperator", "Nor", "Or", "Nand", "And", "Xnor", "Xor",
    "Unequal", "Equal", "NotImplies", "Implies", "NotIfThenElse", "IfThenElse",
    "ZERO", "ONE", "LOGICAL", "ILLOGICAL",
    "not_", "nor", "or_", "nand", "and_", "xnor", "xor", "neq", "eq",
    "nimpl", "impl", "nite", "ite",
    "nor_s", "or_s", "nand_s", "and_s", "xnor_s", "xor_s", "neq_s", "eq_s",
    "nimpl_s", "impl_s", "nite_s", "ite_s",
    "onehot0", "onehot", "majority", "achilles_heel",
    "exists", "forall",
]

__version__ = "1.2"


class Context:
    """
    A context for Boolean variables
    """
    def __init__(self):
        self._cdata = lib.boolexpr_Context_new()

    def __del__(self):
        lib.boolexpr_Context_del(self._cdata)

    @property
    def cdata(self):
        """Return the CFFI CData object."""
        return self._cdata

    def get_var(self, name):
        """Return a Variable with a given name."""
        cdata = lib.boolexpr_Context_get_var(self._cdata, name.encode("ascii"))
        return _bx(cdata)


class BoolExpr:
    """
    Base class for Boolean expressions
    """

    from enum import Enum

    class Kind(Enum):
        """BoolExpr Kind Codes"""
        zero = lib.ZERO
        one = lib.ONE
        log = lib.LOG
        ill = lib.ILL
        comp = lib.COMP
        var = lib.VAR
        nor = lib.NOR
        or_ = lib.OR
        nand = lib.NAND
        and_ = lib.AND
        xnor = lib.XNOR
        xor = lib.XOR
        neq = lib.NEQ
        eq = lib.EQ
        nimpl = lib.NIMPL
        impl = lib.IMPL
        nite = lib.NITE
        ite = lib.ITE

    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_BoolExpr_del(self._cdata)

    def to_ast(self):
        """Convert to an abstract syntax tree (AST)."""
        raise NotImplementedError()

    @classmethod
    def from_ast(cls, ast):
        """Convert an abstract syntax tree (AST) to a BoolExpr."""
        fst, rst = ast[0], ast[1:]
        return _AST[fst](rst)

    @property
    def cdata(self):
        """Return the CFFI CData object."""
        return self._cdata

    def __repr__(self):
        return self.__str__()

    def __bytes__(self):
        c_str = lib.boolexpr_BoolExpr_to_string(self._cdata)
        try:
            return ffi.string(c_str)
        finally:
            lib.boolexpr_String_del(c_str)

    def __str__(self):
        return self.__bytes__().decode('utf-8')

    def __invert__(self):
        """Boolean negation operator

        +-----------+------------+
        | :math:`f` | :math:`f'` |
        +===========+============+
        |         0 |          1 |
        +-----------+------------+
        |         1 |          0 |
        +-----------+------------+
        """
        return not_(self)

    def __or__(self, other):
        """Boolean disjunction (sum, OR) operator

        +-----------+-----------+---------------+
        | :math:`f` | :math:`g` | :math:`f + g` |
        +===========+===========+===============+
        |         0 |         0 |             0 |
        +-----------+-----------+---------------+
        |         0 |         1 |             1 |
        +-----------+-----------+---------------+
        |         1 |         0 |             1 |
        +-----------+-----------+---------------+
        |         1 |         1 |             1 |
        +-----------+-----------+---------------+
        """
        return or_(self, other)

    def __ror__(self, other):
        return or_(other, self)

    def __and__(self, other):
        r"""Boolean conjunction (product, AND) operator

        +-----------+-----------+-------------------+
        | :math:`f` | :math:`g` | :math:`f \cdot g` |
        +===========+===========+===================+
        |         0 |         0 |                 0 |
        +-----------+-----------+-------------------+
        |         0 |         1 |                 0 |
        +-----------+-----------+-------------------+
        |         1 |         0 |                 0 |
        +-----------+-----------+-------------------+
        |         1 |         1 |                 1 |
        +-----------+-----------+-------------------+
        """
        return and_(self, other)

    def __rand__(self, other):
        return and_(other, self)

    def __xor__(self, other):
        r"""Boolean exclusive or (XOR) operator

        +-----------+-----------+--------------------+
        | :math:`f` | :math:`g` | :math:`f \oplus g` |
        +===========+===========+====================+
        |         0 |         0 |                  0 |
        +-----------+-----------+--------------------+
        |         0 |         1 |                  1 |
        +-----------+-----------+--------------------+
        |         1 |         0 |                  1 |
        +-----------+-----------+--------------------+
        |         1 |         1 |                  0 |
        +-----------+-----------+--------------------+
        """
        return xor(self, other)

    def __rxor__(self, other):
        return xor(other, self)

    @property
    def kind(self):
        """Return the Kind code.

        The expression "kind" is one byte of information.
        See ``include/boolexpr/boolexpr.h`` for the full table of kinds codes.
        For convenience, this value is wrapped by a Python Enum.
        """
        return self.Kind(lib.boolexpr_BoolExpr_kind(self._cdata))

    def depth(self):
        """Return the depth of the expression.

        1. An ``Atom`` has zero depth.
        2. An ``Operator`` has depth equal to the maximum depth of its
           arguments plus one.
        """
        return lib.boolexpr_BoolExpr_depth(self._cdata)

    def size(self):
        """Return the size of the expression.

        1. An ``Atom`` has size one.
        2. An ``Operator`` has size equal to the sum of its arguments' sizes
           plus one.
        """
        return lib.boolexpr_BoolExpr_size(self._cdata)

    def is_cnf(self):
        """Return ``True`` if the expression is in conjunctive normal form (CNF).

        A CNF is defined as a conjunction of clauses,
        where a clause is defined as either a ``Literal``,
        or a disjunction of literals.

        Boolean True (``And`` identity) is also considered to be a CNF.
        """
        return bool(lib.boolexpr_BoolExpr_is_cnf(self._cdata))

    def is_dnf(self):
        """
        Return ``True`` if the expression is in disjunctive normal form (DNF).

        A DNF is defined as a disjunction of clauses,
        where a clause is defined as either a ``Literal``,
        or a disjunction of literals.

        Boolean False (``Or`` identity) is also considered to be a DNF.
        """
        return bool(lib.boolexpr_BoolExpr_is_dnf(self._cdata))

    def simplify(self):
        """Return a simplified expression.

        Simplification uses Boolean algebra identities to reduce the size
        of the expression.
        """
        return _bx(lib.boolexpr_BoolExpr_simplify(self._cdata))

    def to_binop(self):
        """Return an expression that uses only binary operators."""
        return _bx(lib.boolexpr_BoolExpr_to_binop(self._cdata))

    def to_latop(self):
        """
        Convert all operators to ``Or`` / ``And`` (lattice operator) form.
        """
        return _bx(lib.boolexpr_BoolExpr_to_latop(self._cdata))

    def to_posop(self):
        r"""Return an expression with NOT bubbles pushed down through dual ops.

        Specifically, perform the following transformations:

        .. math::

           \overline{a + b + \ldots} &\iff \overline{a\vphantom{b}} \cdot \overline{b} \cdot \ldots

           \overline{a \cdot b \cdot \ldots} &\iff \overline{a\vphantom{b}} + \overline{b} + \ldots

           \overline{a \oplus b \oplus \ldots} &\iff \overline{a} \oplus b \oplus \ldots

           \overline{eq(a, b, \ldots)} &\iff eq(\overline{a}, b, \ldots)

           \overline{p \implies q} &\iff p \cdot \overline{q}

           \overline{ite(s, d1, d0)} &\iff ite(s, \overline{d1}, \overline{d0})
        """
        return _bx(lib.boolexpr_BoolExpr_to_posop(self._cdata))

    def tseytin(self, ctx, auxvarname="a"):
        """Return the Tseytin transformation.

        The ``ctx`` parameter is a ``Context`` object that will be used to
        store auxiliary variables.

        The ``auxvarname`` parameter is the prefix of auxiliary variable names.
        The suffix will be in the form ``_0``, ``_1``, etc.
        """
        name = auxvarname.encode("ascii")
        return _bx(lib.boolexpr_BoolExpr_tseytin(self._cdata, ctx.cdata, name))

    def compose(self, var2bx):
        r"""
        Substitute a subset of support variables with other Boolean expressions.

        Returns a new expression: :math:`f(g_i, \ldots)`

        :math:`f_1 \: | \: x_i = f_2`
        """
        num = len(var2bx)
        vars_ = ffi.new("void * []", num)
        bxs = ffi.new("void * []", num)
        for i, (var, bx) in enumerate(var2bx.items()):
            vars_[i] = _expect_var(var).cdata
            bxs[i] = _expect_bx(bx).cdata
        return _bx(lib.boolexpr_BoolExpr_compose(self._cdata, num, vars_, bxs))

    def restrict(self, point):
        r"""
        Restrict a subset of support variables to :math:`\{0, 1\}`.

        Returns a new function: :math:`f(x_i, \ldots)`

        :math:`f \: | \: x_i = b`
        """
        num = len(point)
        vars_ = ffi.new("void * []", num)
        consts = ffi.new("void * []", num)
        for i, (var, const) in enumerate(point.items()):
            vars_[i] = _expect_var(var).cdata
            consts[i] = _expect_const(const).cdata
        return _bx(lib.boolexpr_BoolExpr_restrict(self._cdata, num, vars_, consts))

    def sat(self):
        """Return a tuple (sat, point).

        The sat value is ``True`` if the expression is satisfiable.
        If the expression is not satisfiable, the point will be ``None``.
        Otherwise, it will return a satisfying input point.
        """
        soln = lib.boolexpr_BoolExpr_sat(self._cdata)
        try:
            sat = bool(lib.boolexpr_Soln_first(soln))
            if not sat:
                return (False, None)
            c_point = lib.boolexpr_Soln_second(soln)
        finally:
            lib.boolexpr_Soln_del(soln)
        try:
            return (True, _point(c_point))
        finally:
            lib.boolexpr_Point_del(c_point)

    def iter_sat(self):
        """Iterate through all satisfying input points."""
        it = lib.boolexpr_SatIter_new(self._cdata)
        try:
            while True:
                val = lib.boolexpr_SatIter_val(it)
                if val == ffi.NULL:
                    break
                yield _point(val)
                lib.boolexpr_SatIter_next(it)
        finally:
            lib.boolexpr_SatIter_del(it)

    def to_cnf(self):
        """Convert the expression to conjunctive normal form (CNF)."""
        return _bx(lib.boolexpr_BoolExpr_to_cnf(self._cdata))

    def to_dnf(self):
        """Convert the expression to disjunctive normal form (DNF)."""
        return _bx(lib.boolexpr_BoolExpr_to_dnf(self._cdata))

    def to_nnf(self):
        """Convert the expression to negation normal form (NNF).

        Negation formal form is defined as:

        * Only ``Or`` or ``And`` operators
        * All NOT bubbles are pushed down to the literals
        * Simple
        """
        return _bx(lib.boolexpr_BoolExpr_to_nnf(self._cdata))

    def equiv(self, other):
        """Return True if the two expressions are formally equivalent.

        .. note:: While in practice this check can be quite fast,
                  SAT is an NP-complete problem, so some inputs will require
                  exponential runtime.
        """
        return bool(lib.boolexpr_BoolExpr_equiv(self._cdata, other.cdata))

    def support(self):
        """Return the support set of the expression."""
        c_varset = lib.boolexpr_BoolExpr_support(self._cdata)
        try:
            return _varset(c_varset)
        finally:
            lib.boolexpr_VarSet_del(c_varset)

    def smoothing(self, xs):
        r"""Return the smoothing over a sequence of N variables.

        The *xs* argument is a sequence of :math:`N` Boolean variables.

        The *smoothing* of :math:`f(x_1, x_2, \dots, x_i, \dots, x_n)` with
        respect to variable :math:`x_i` is:
        :math:`S_{x_i}(f) = f_{x_i} + f_{x_i'}`

        This is the same as the existential quantification operator:
        :math:`\exists \{x_1, x_2, \dots\} \: f`
        """
        if isinstance(xs, Variable):
            xs = [xs]
        num = len(xs)
        vars_ = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            vars_[i] = _expect_var(x).cdata
        return _bx(lib.boolexpr_BoolExpr_smoothing(self._cdata, num, vars_))

    def consensus(self, xs):
        r"""Return the consensus over a sequence of N variables.

        The *xs* argument is a sequence of :math:`N` Boolean variables.

        The *consensus* of :math:`f(x_1, x_2, \dots, x_i, \dots, x_n)` with
        respect to variable :math:`x_i` is:
        :math:`C_{x_i}(f) = f_{x_i} \cdot f_{x_i'}`

        This is the same as the universal quantification operator:
        :math:`\forall \{x_1, x_2, \dots\} \: f`
        """
        if isinstance(xs, Variable):
            xs = [xs]
        num = len(xs)
        vars_ = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            vars_[i] = _expect_var(x).cdata
        return _bx(lib.boolexpr_BoolExpr_consensus(self._cdata, num, vars_))

    def derivative(self, xs):
        r"""Return the derivative over a sequence of N variables.

        The *xs* argument is a sequence of :math:`N` Boolean variables.

        The *derivative* of :math:`f(x_1, x_2, \dots, x_i, \dots, x_n)` with
        respect to variable :math:`x_i` is:
        :math:`\frac{\partial}{\partial x_i} f = f_{x_i} \oplus f_{x_i'}`

        This is also known as the Boolean *difference*.
        """
        if isinstance(xs, Variable):
            xs = [xs]
        num = len(xs)
        vars_ = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            vars_[i] = _expect_var(x).cdata
        return _bx(lib.boolexpr_BoolExpr_derivative(self._cdata, num, vars_))

    def iter_dfs(self):
        """Iterate through all expression nodes in DFS order."""
        it = lib.boolexpr_DfsIter_new(self._cdata)
        try:
            while True:
                val = lib.boolexpr_DfsIter_val(it)
                if val == ffi.NULL:
                    break
                yield _bx(val)
                lib.boolexpr_DfsIter_next(it)
        finally:
            lib.boolexpr_DfsIter_del(it)

    def iter_domain(self):
        """Iterate through all points in the domain."""
        it = lib.boolexpr_DomainIter_new(self._cdata)
        try:
            while True:
                val = lib.boolexpr_DomainIter_val(it)
                if val == ffi.NULL:
                    break
                yield _point(val)
                lib.boolexpr_DomainIter_next(it)
        finally:
            lib.boolexpr_DomainIter_del(it)

    def iter_cfs(self, xs):
        """Iterate through all cofactors over a sequence of variables."""
        if isinstance(xs, Variable):
            xs = [xs]
        num = len(xs)
        vars_ = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            vars_[i] = _expect_var(x).cdata
        it = lib.boolexpr_CofactorIter_new(self._cdata, num, vars_)
        try:
            while True:
                val = lib.boolexpr_CofactorIter_val(it)
                if val == ffi.NULL:
                    break
                yield _bx(val)
                lib.boolexpr_CofactorIter_next(it)
        finally:
            lib.boolexpr_CofactorIter_del(it)


class Atom(BoolExpr):
    """Boolean Atom"""

class Constant(Atom):
    """Boolean Constant Atom"""

class Known(Constant):
    """Boolean Known Constant"""


class Zero(Known):
    """Boolean Zero"""
    def __bool__(self):
        return False

    def __int__(self):
        return 0

    def to_ast(self):
        return (self.kind, )

class One(Known):
    """Boolean One"""
    def __bool__(self):
        return True

    def __int__(self):
        return 1

    def to_ast(self):
        return (self.kind, )


class Unknown(Constant):
    """Boolean Unknown Constant"""

class Logical(Unknown):
    """Boolean Logical Unknown"""
    def to_ast(self):
        return (self.kind, )

class Illogical(Unknown):
    """Boolean Illogical Unknown"""
    def to_ast(self):
        return (self.kind, )


class Literal(Atom):
    """Boolean Literal Atom"""

class Complement(Literal):
    """Boolean Complement"""
    def to_ast(self):
        return (
            self.kind,
            int(ffi.cast("uintptr_t", lib.boolexpr_Literal_ctx(self._cdata))),
            str(~self),
        )

class Variable(Literal):
    """Boolean Variable"""
    def to_ast(self):
        return (
            self.kind,
            int(ffi.cast("uintptr_t", lib.boolexpr_Literal_ctx(self._cdata))),
            self.__str__()
        )


class Operator(BoolExpr):
    """Boolean Operator"""

    def to_ast(self):
        return (self.kind, ) + tuple(arg.to_ast() for arg in self.args)

    @property
    def simple(self):
        """Return ``True`` if the operator is simple.

        An operator is only deemed simple if it has been returned by the
        ``simplify`` method.
        """
        return bool(lib.boolexpr_Operator_simple(self._cdata))

    @property
    def args(self):
        """Return a tuple of the operator's arguments."""
        c_vec = lib.boolexpr_Operator_args(self._cdata)
        try:
            return tuple(_bxlist(c_vec))
        finally:
            lib.boolexpr_Vec_del(c_vec)

    def is_clause(self):
        """Return ``True`` if the operator is a clause.

        A *clause* is defined as having only ``Literal`` arguments.
        """
        return bool(lib.boolexpr_Operator_is_clause(self._cdata))


class LatticeOperator(Operator):
    """Boolean Lattice Operator"""

class Nor(Operator):
    """Boolean Nor Operator"""

class Or(LatticeOperator):
    """Boolean Or Operator"""

class Nand(Operator):
    """Boolean Nand Operator"""

class And(LatticeOperator):
    """Boolean And Operator"""

class Xnor(Operator):
    """Boolean Xnor Operator"""

class Xor(Operator):
    """Boolean Xor Operator"""

class Unequal(Operator):
    """Boolean Unequal Operator"""

class Equal(Operator):
    """Boolean Equal Operator"""

class NotImplies(Operator):
    """Boolean Not Implies Operator"""

class Implies(Operator):
    """Boolean Implies Operator"""

class NotIfThenElse(Operator):
    """Boolean Not IfThenElse Operator"""

class IfThenElse(Operator):
    """Boolean IfThenElse Operator"""


ZERO = Zero(lib.boolexpr_zero())
ONE = One(lib.boolexpr_one())
LOGICAL = Logical(lib.boolexpr_logical())
ILLOGICAL = Illogical(lib.boolexpr_illogical())


def not_(arg):
    """Boolean Not operator."""
    _, c_args = _convert_args((arg, ))
    return _bx(lib.boolexpr_not(c_args[0]))

def nor(*args):
    """Boolean Nor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_nor(num, c_args))

def or_(*args):
    """Boolean Or operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_or(num, c_args))

def nand(*args):
    """Boolean Nand operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_nand(num, c_args))

def and_(*args):
    """Boolean And operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_and(num, c_args))

def xnor(*args):
    """Boolean Xnor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_xnor(num, c_args))

def xor(*args):
    """Boolean Xor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_xor(num, c_args))

def neq(*args):
    """Boolean Unequal operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_neq(num, c_args))

def eq(*args):
    """Boolean Equal operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_eq(num, c_args))

def nimpl(p, q):
    """Boolean NotImplies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_nimpl(c_args[0], c_args[1]))

def impl(p, q):
    """Boolean Implies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl(c_args[0], c_args[1]))

def nite(s, d1, d0):
    """Boolean NotIfThenElse operator."""
    _, c_args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_nite(c_args[0], c_args[1], c_args[2]))

def ite(s, d1, d0):
    """Boolean IfThenElse operator."""
    _, c_args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite(c_args[0], c_args[1], c_args[2]))

def nor_s(*args):
    """Simplifying Boolean Nor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_nor_s(num, c_args))

def or_s(*args):
    """Simplifying Boolean Or operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_or_s(num, c_args))

def nand_s(*args):
    """Simplifying Boolean Nand operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_nand_s(num, c_args))

def and_s(*args):
    """Simplifying Boolean And operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_and_s(num, c_args))

def xnor_s(*args):
    """Simplifying Boolean Xnor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_xnor_s(num, c_args))

def xor_s(*args):
    """Simplifying Boolean Xor operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_xor_s(num, c_args))

def neq_s(*args):
    """Simplifying Boolean Unequal operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_neq_s(num, c_args))

def eq_s(*args):
    """Simplifying Boolean Equal operator."""
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_eq_s(num, c_args))

def nimpl_s(p, q):
    """Simplifying Boolean Implies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_nimpl_s(c_args[0], c_args[1]))

def impl_s(p, q):
    """Simplifying Boolean Implies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl_s(c_args[0], c_args[1]))

def nite_s(s, d1, d0):
    """Simplifying Boolean IfThenElse operator."""
    _, c_args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_nite_s(c_args[0], c_args[1], c_args[2]))

def ite_s(s, d1, d0):
    """Simplifying Boolean IfThenElse operator."""
    _, c_args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite_s(c_args[0], c_args[1], c_args[2]))


def onehot0(*args):
    """
    Return an expression that means
    "at most one input function is true".
    """
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_onehot0(num, c_args))


def onehot(*args):
    """
    Return an expression that means
    "exactly one input function is true".
    """
    num, c_args = _convert_args(args)
    return _bx(lib.boolexpr_onehot(num, c_args))


def majority(*args, conj=False):
    """
    Return an expression that means
    "the majority of input functions are true".

    If *conj* is ``True``, return a CNF.
    Otherwise, return a DNF.
    """
    import itertools
    terms = list()
    if conj:
        for xs in itertools.combinations(args, (len(args) + 1) // 2):
            terms.append(or_(*xs))
        return and_(*terms)
    else:
        for xs in itertools.combinations(args, len(args) // 2 + 1):
            terms.append(and_(*xs))
        return or_(*terms)


def achilles_heel(*args):
    r"""
    Return the Achille's Heel function, defined as:
    :math:`\prod_{i=0}^{n/2-1}{X_{2i} + X_{2i+1}}`.
    """
    num = len(args)
    if num & 1:
        fstr = "expected an even number of arguments, got {}"
        raise ValueError(fstr.format(num))
    return and_(*[or_(args[2*i], args[2*i+1]) for i in range(num // 2)])


def exists(xs, f):
    """
    Return an expression that means
    "there exists a variable in *xs* such that *f* true."

    This is identical to ``f.smoothing(xs)``.
    """
    return f.smoothing(xs)


def forall(xs, f):
    """
    Return an expression that means
    "for all variables in *xs*, *f* is true."

    This is identical to ``f.consensus(xs)``.
    """
    return f.consensus(xs)


_LITS = dict()

_KIND2CONST = {
    lib.ZERO : ZERO,
    lib.ONE  : ONE,
    lib.LOG  : LOGICAL,
    lib.ILL  : ILLOGICAL,
}

_KIND2LIT = {
    lib.COMP : Complement,
    lib.VAR  : Variable,
}

_KIND2OTHER = {
    lib.NOR   : Nor,
    lib.OR    : Or,
    lib.NAND  : Nand,
    lib.AND   : And,
    lib.XNOR  : Xnor,
    lib.XOR   : Xor,
    lib.NEQ   : Unequal,
    lib.EQ    : Equal,
    lib.NIMPL : NotImplies,
    lib.IMPL  : Implies,
    lib.NITE  : NotIfThenElse,
    lib.ITE   : IfThenElse,
}

def _var(ctx_num, name):
    ctx = ffi.cast("void *", ctx_num)
    return _bx(lib.boolexpr_Context_get_var(ctx, name.encode("ascii")))

_AST = {
    BoolExpr.Kind.zero  : lambda _: ZERO,
    BoolExpr.Kind.one   : lambda _: ONE,
    BoolExpr.Kind.log   : lambda _: LOGICAL,
    BoolExpr.Kind.zero  : lambda _: ILLOGICAL,
    BoolExpr.Kind.comp  : lambda args: ~_var(*args),
    BoolExpr.Kind.var   : lambda args: _var(*args),
    BoolExpr.Kind.nor   : lambda asts: nor(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.or_   : lambda asts: or_(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.nand  : lambda asts: nand(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.and_  : lambda asts: and_(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.xnor  : lambda asts: xnor(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.xor   : lambda asts: xor(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.neq   : lambda asts: neq(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.eq    : lambda asts: eq(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.nimpl : lambda asts: nimpl(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.impl  : lambda asts: impl(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.nite  : lambda asts: nite(*map(BoolExpr.from_ast, asts)),
    BoolExpr.Kind.ite   : lambda asts: ite(*map(BoolExpr.from_ast, asts)),
}


def _expect_bx(obj):
    """Return a BoolExpr or raise TypeError."""
    if obj == 0:
        return ZERO
    elif obj == 1:
        return ONE
    elif obj == "x" or obj == "X":
        return LOGICAL
    elif obj == "?":
        return ILLOGICAL
    elif isinstance(obj, BoolExpr):
        return obj
    else:
        raise TypeError("Expected obj to be a BoolExpr")


def _expect_const(obj):
    """Return a Constant, or raise TypeError."""
    if obj == 0:
        return ZERO
    elif obj == 1:
        return ONE
    elif obj == "x" or obj == "X":
        return LOGICAL
    elif obj == "?":
        return ILLOGICAL
    elif isinstance(obj, Constant):
        return obj
    else:
        raise TypeError("Expected obj to be a Constant")


def _expect_var(obj):
    """Return a Variable, or raise TypeError."""
    if isinstance(obj, Variable):
        return obj
    else:
        raise TypeError("Expected obj to be a Variable")


def _convert_args(args):
    """Convert a sequence of Python BoolExpr to a C [BoolExpr]."""
    num = len(args)
    c_args = ffi.new("void const * []", num)
    for i, arg in enumerate(args):
        c_args[i] = _expect_bx(arg).cdata
    return num, c_args


def _bx(cbx):
    kind = lib.boolexpr_BoolExpr_kind(cbx)
    if kind in _KIND2CONST:
        lib.boolexpr_BoolExpr_del(cbx)
        return _KIND2CONST[kind]
    if kind in _KIND2LIT:
        key = (int(ffi.cast("uintptr_t", lib.boolexpr_Literal_ctx(cbx))),
               lib.boolexpr_Literal_id(cbx))
        try:
            lit = _LITS[key]
        except KeyError:
            lit = _LITS[key] = _KIND2LIT[kind](cbx)
        else:
            lib.boolexpr_BoolExpr_del(cbx)
        return lit
    return _KIND2OTHER[kind](cbx)


def _bxlist(c_vec):
    """Return a [BoolExpr], from a C Vec."""
    vec = list()
    lib.boolexpr_Vec_iter(c_vec)
    while True:
        val = lib.boolexpr_Vec_val(c_vec)
        if val == ffi.NULL:
            break
        vec.append(_bx(val))
        lib.boolexpr_Vec_next(c_vec)
    return vec


def _varset(c_varset):
    """Return a {Variable}, from a C VarSet."""
    vars_ = set()
    lib.boolexpr_VarSet_iter(c_varset)
    while True:
        val = lib.boolexpr_VarSet_val(c_varset)
        if val == ffi.NULL:
            break
        vars_.add(_bx(val))
        lib.boolexpr_VarSet_next(c_varset)
    return vars_


def _point(c_point):
    """Return a {Variable: Constant}, from a C Point."""
    point = dict()
    lib.boolexpr_Point_iter(c_point)
    while True:
        key = lib.boolexpr_Point_key(c_point)
        if key == ffi.NULL:
            break
        val = lib.boolexpr_Point_val(c_point)
        point[_bx(key)] = _bx(val)
        lib.boolexpr_Point_next(c_point)
    return point
