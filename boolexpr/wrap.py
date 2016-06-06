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
CFFI _boolexpr module wrapper
"""


# Ignore several instances of accessing obj._cdata
# pylint: disable=protected-access

# Some of the wrapper classes have only __dunder__ methods
# pylint: disable=too-few-public-methods

# This module needs to be a bit long
# pylint: disable=too-many-lines


import collections
import enum
import itertools
import operator
from functools import reduce

# pylint: disable=no-name-in-module
from ._boolexpr import ffi
from ._boolexpr import lib
# pylint: enable=no-name-in-module
from .util import clog2


class _String:
    """
    Wrap C char *
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_String_del(self._cdata)

    def __bytes__(self):
        return ffi.string(self._cdata)


class _Vec:
    """
    Wrap C Vec
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_Vec_del(self._cdata)

    def __iter__(self):
        lib.boolexpr_Vec_iter(self._cdata)
        while True:
            val = lib.boolexpr_Vec_val(self._cdata)
            if val == ffi.NULL:
                break
            yield _bx(val)
            lib.boolexpr_Vec_next(self._cdata)


class _VarSet:
    """
    Wrap C VarSet
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_VarSet_del(self._cdata)

    def __iter__(self):
        lib.boolexpr_VarSet_iter(self._cdata)
        while True:
            val = lib.boolexpr_VarSet_val(self._cdata)
            if val == ffi.NULL:
                break
            yield _bx(val)
            lib.boolexpr_VarSet_next(self._cdata)


class _Point:
    """
    Wrap C Point
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_Point_del(self._cdata)

    def __iter__(self):
        lib.boolexpr_Point_iter(self._cdata)
        while True:
            key = lib.boolexpr_Point_key(self._cdata)
            if key == ffi.NULL:
                break
            val = lib.boolexpr_Point_val(self._cdata)
            yield _bx(key), _bx(val)
            lib.boolexpr_Point_next(self._cdata)


class _Soln:
    """
    Wrap C Soln
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_Soln_del(self._cdata)

    @property
    def sat(self):
        """Return True if the solution is satisfiable."""
        return bool(lib.boolexpr_Soln_first(self._cdata))

    @property
    def point(self):
        """Return a satisfying input point, or None."""
        if not self.sat:
            return None
        return dict(_Point(lib.boolexpr_Soln_second(self._cdata)))

    @property
    def t(self):
        """Return the solution formatted as a tuple."""
        return (self.sat, self.point)


class _DfsIter:
    """
    Wrap C DfsIter
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_DfsIter_del(self._cdata)

    def __iter__(self):
        while True:
            val = lib.boolexpr_DfsIter_val(self._cdata)
            if val == ffi.NULL:
                break
            yield _bx(val)
            lib.boolexpr_DfsIter_next(self._cdata)


class _SatIter:
    """
    Wrap C SatIter
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_SatIter_del(self._cdata)

    def __iter__(self):
        while True:
            val = lib.boolexpr_SatIter_val(self._cdata)
            if val == ffi.NULL:
                break
            yield dict(_Point(val))
            lib.boolexpr_SatIter_next(self._cdata)


class _PointsIter:
    """
    Wrap C PointsIter
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_PointsIter_del(self._cdata)

    def __iter__(self):
        while True:
            val = lib.boolexpr_PointsIter_val(self._cdata)
            if val == ffi.NULL:
                break
            yield dict(_Point(val))
            lib.boolexpr_PointsIter_next(self._cdata)


class _DomainIter:
    """
    Wrap C DomainIter
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_DomainIter_del(self._cdata)

    def __iter__(self):
        while True:
            val = lib.boolexpr_DomainIter_val(self._cdata)
            if val == ffi.NULL:
                break
            yield dict(_Point(val))
            lib.boolexpr_DomainIter_next(self._cdata)


class _CofactorIter:
    """
    Wrap C CofactorIter
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_CofactorIter_del(self._cdata)

    def __iter__(self):
        while True:
            val = lib.boolexpr_CofactorIter_val(self._cdata)
            if val == ffi.NULL:
                break
            yield _bx(val)
            lib.boolexpr_CofactorIter_next(self._cdata)


class _ArrayPair:
    """
    Wrap C ArrayPair
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_ArrayPair_del(self._cdata)

    @property
    def fst(self):
        """Return the first element of the pair."""
        return Array(lib.boolexpr_ArrayPair_fst(self._cdata))

    @property
    def snd(self):
        """Return the second element of the pair."""
        return Array(lib.boolexpr_ArrayPair_snd(self._cdata))

    @property
    def t(self):
        """Return the pair formatted as a tuple."""
        return (self.fst, self.snd)


class Context:
    """
    A context for Boolean variables
    """
    def __init__(self):
        self._cdata = lib.boolexpr_Context_new()

    def __del__(self):
        lib.boolexpr_Context_del(self._cdata)

    def get_var(self, name):
        """Return a Variable with a given name."""
        cdata = lib.boolexpr_Context_get_var(self._cdata, name.encode("ascii"))
        return _bx(cdata)

    def get_vars(self, name, *dims):
        """Return a multi-dimensional array of variables.

        The *name* argument is a ``str``.

        The variadic *dims* input is a sequence of dimension specs.
        A dimension spec is a two-tuple: (start index, stop index).
        If a dimension is given as a single ``int``,
        it will be converted to ``(0, stop)``.

        The dimension starts at index ``start``,
        and increments by one up to, but not including, ``stop``.
        This follows the Python slice convention.
        """
        shape = _dims2shape(*dims)
        objs = list()
        for indices in itertools.product(*[range(i, j) for i, j in shape]):
            suffix = "[" + ",".join(str(idx) for idx in indices) + "]"
            objs.append(self.get_var(name + suffix))
        return array(objs, shape)


class BoolExpr:
    """
    Wrap boolexpr::BoolExpr class
    """

    class Kind(enum.Enum):
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

    @classmethod
    def from_ast(cls, ast):
        """Convert an abstract syntax tree (AST) to a BoolExpr."""
        fst, rst = ast[0], ast[1:]
        return _AST[fst](rst)

    def __repr__(self):
        return self.__str__()

    def __bytes__(self):
        return bytes(_String(lib.boolexpr_BoolExpr_to_string(self._cdata)))

    def __str__(self):
        return self.__bytes__().decode("utf-8")

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

    def __add__(self, other):
        return array([self]) + other

    def __radd__(self, other):
        return array([other]) + self

    def __mul__(self, num):
        return array([self]) * num

    def __rmul__(self, num):
        return self.__mul__(num)

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
        return _bx(lib.boolexpr_BoolExpr_tseytin(self._cdata, ctx._cdata, name))

    def compose(self, var2bx):
        r"""
        Substitute a subset of support variables with other Boolean expressions.

        Returns a new expression: :math:`f(g_i, \ldots)`

        :math:`f_1 \: | \: x_i = f_2`
        """
        num = len(var2bx)
        c_vars = ffi.new("void * []", num)
        c_bxs = ffi.new("void * []", num)
        for i, (var, bx) in enumerate(var2bx.items()):
            c_vars[i] = _expect_var(var)._cdata
            c_bxs[i] = _expect_bx(bx)._cdata
        return _bx(lib.boolexpr_BoolExpr_compose(self._cdata, num, c_vars, c_bxs))

    def restrict(self, point):
        r"""
        Restrict a subset of support variables to :math:`\{0, 1\}`.

        Returns a new function: :math:`f(x_i, \ldots)`

        :math:`f \: | \: x_i = b`
        """
        num = len(point)
        c_vars = ffi.new("void * []", num)
        c_consts = ffi.new("void * []", num)
        for i, (var, const) in enumerate(point.items()):
            c_vars[i] = _expect_var(var)._cdata
            c_consts[i] = _expect_const(const)._cdata
        return _bx(lib.boolexpr_BoolExpr_restrict(self._cdata, num, c_vars, c_consts))

    def sat(self):
        """Return a tuple (sat, point).

        The sat value is ``True`` if the expression is satisfiable.
        If the expression is not satisfiable, the point will be ``None``.
        Otherwise, it will return a satisfying input point.
        """
        return _Soln(lib.boolexpr_BoolExpr_sat(self._cdata)).t

    def iter_sat(self):
        """Iterate through all satisfying input points."""
        yield from _SatIter(lib.boolexpr_SatIter_new(self._cdata))

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
        return bool(lib.boolexpr_BoolExpr_equiv(self._cdata, other._cdata))

    def support(self):
        """Return the support set of the expression."""
        return set(_VarSet(lib.boolexpr_BoolExpr_support(self._cdata)))

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
        c_vars = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            c_vars[i] = _expect_var(x)._cdata
        return _bx(lib.boolexpr_BoolExpr_smoothing(self._cdata, num, c_vars))

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
        c_vars = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            c_vars[i] = _expect_var(x)._cdata
        return _bx(lib.boolexpr_BoolExpr_consensus(self._cdata, num, c_vars))

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
        c_vars = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            c_vars[i] = _expect_var(x)._cdata
        return _bx(lib.boolexpr_BoolExpr_derivative(self._cdata, num, c_vars))

    def iter_dfs(self):
        """Iterate through all expression nodes in DFS order."""
        yield from _DfsIter(lib.boolexpr_DfsIter_new(self._cdata))

    def iter_domain(self):
        """Iterate through all points in the domain."""
        yield from _DomainIter(lib.boolexpr_DomainIter_new(self._cdata))

    def iter_cfs(self, xs):
        """Iterate through all cofactors over a sequence of variables."""
        if isinstance(xs, Variable):
            xs = [xs]
        num = len(xs)
        c_vars = ffi.new("void * []", num)
        for i, x in enumerate(xs):
            c_vars[i] = _expect_var(x)._cdata
        cdata = lib.boolexpr_CofactorIter_new(self._cdata, num, c_vars)
        yield from _CofactorIter(cdata)


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
        return tuple(_Vec(lib.boolexpr_Operator_args(self._cdata)))

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


def iter_points(xs):
    """Iterate through all points in a Boolean space."""
    if isinstance(xs, Variable):
        xs = [xs]
    num = len(xs)
    c_vars = ffi.new("void * []", num)
    for i, x in enumerate(xs):
        c_vars[i] = _expect_var(x)._cdata
    yield from _PointsIter(lib.boolexpr_PointsIter_new(len(c_vars), c_vars))


def not_(arg):
    """Boolean Not operator."""
    _, c_bxs = _convert_args((arg, ))
    return _bx(lib.boolexpr_not(c_bxs[0]))

def nor(*args):
    """Boolean Nor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_nor(num, c_bxs))

def or_(*args):
    """Boolean Or operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_or(num, c_bxs))

def nand(*args):
    """Boolean Nand operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_nand(num, c_bxs))

def and_(*args):
    """Boolean And operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_and(num, c_bxs))

def xnor(*args):
    """Boolean Xnor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_xnor(num, c_bxs))

def xor(*args):
    """Boolean Xor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_xor(num, c_bxs))

def neq(*args):
    """Boolean Unequal operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_neq(num, c_bxs))

def eq(*args):
    """Boolean Equal operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_eq(num, c_bxs))

def nimpl(p, q):
    """Boolean NotImplies operator."""
    _, c_bxs = _convert_args((p, q))
    return _bx(lib.boolexpr_nimpl(c_bxs[0], c_bxs[1]))

def impl(p, q):
    """Boolean Implies operator."""
    _, c_bxs = _convert_args((p, q))
    return _bx(lib.boolexpr_impl(c_bxs[0], c_bxs[1]))

def nite(s, d1, d0):
    """Boolean NotIfThenElse operator."""
    _, c_bxs = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_nite(c_bxs[0], c_bxs[1], c_bxs[2]))

def ite(s, d1, d0):
    """Boolean IfThenElse operator."""
    _, c_bxs = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite(c_bxs[0], c_bxs[1], c_bxs[2]))

def nor_s(*args):
    """Simplifying Boolean Nor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_nor_s(num, c_bxs))

def or_s(*args):
    """Simplifying Boolean Or operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_or_s(num, c_bxs))

def nand_s(*args):
    """Simplifying Boolean Nand operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_nand_s(num, c_bxs))

def and_s(*args):
    """Simplifying Boolean And operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_and_s(num, c_bxs))

def xnor_s(*args):
    """Simplifying Boolean Xnor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_xnor_s(num, c_bxs))

def xor_s(*args):
    """Simplifying Boolean Xor operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_xor_s(num, c_bxs))

def neq_s(*args):
    """Simplifying Boolean Unequal operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_neq_s(num, c_bxs))

def eq_s(*args):
    """Simplifying Boolean Equal operator."""
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_eq_s(num, c_bxs))

def nimpl_s(p, q):
    """Simplifying Boolean Implies operator."""
    _, c_bxs = _convert_args((p, q))
    return _bx(lib.boolexpr_nimpl_s(c_bxs[0], c_bxs[1]))

def impl_s(p, q):
    """Simplifying Boolean Implies operator."""
    _, c_bxs = _convert_args((p, q))
    return _bx(lib.boolexpr_impl_s(c_bxs[0], c_bxs[1]))

def nite_s(s, d1, d0):
    """Simplifying Boolean IfThenElse operator."""
    _, c_bxs = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_nite_s(c_bxs[0], c_bxs[1], c_bxs[2]))

def ite_s(s, d1, d0):
    """Simplifying Boolean IfThenElse operator."""
    _, c_bxs = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite_s(c_bxs[0], c_bxs[1], c_bxs[2]))


def onehot0(*args):
    """
    Return an expression that means
    "at most one input function is true".
    """
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_onehot0(num, c_bxs))


def onehot(*args):
    """
    Return an expression that means
    "exactly one input function is true".
    """
    num, c_bxs = _convert_args(args)
    return _bx(lib.boolexpr_onehot(num, c_bxs))


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

_KIND2OP = {
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

# pylint: disable=bad-builtin
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
# pylint: enable=bad-builtin


def _expect_array(obj):
    """Return an ndarray, or raise TypeError."""
    if obj == 0:
        return array([ZERO])
    elif obj == 1:
        return array([ONE])
    elif obj == "x" or obj == "X":
        return array([LOGICAL])
    elif obj == "?":
        return array([ILLOGICAL])
    elif isinstance(obj, BoolExpr):
        return array([obj])
    elif isinstance(obj, ndarray):
        return obj
    else:
        raise TypeError("Expected obj to be an ndarray")


def _expect_bx(obj):
    """Return a BoolExpr, or raise TypeError."""
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
        c_args[i] = _expect_bx(arg)._cdata
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
    return _KIND2OP[kind](cbx)


class Array:
    """
    Wrap boolexpr::Array class
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_Array_del(self._cdata)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return "(" + ", ".join(str(item) for item in self) + ")"

    def __len__(self):
        return lib.boolexpr_Array_size(self._cdata)

    def __getitem__(self, key):
        if isinstance(key, slice):
            start, stop, _ = self._key2indices(key)
            cdata = lib.boolexpr_Array_getslice(self._cdata, start, stop)
            return Array(cdata)
        index = self._key2index(key)
        return _bx(lib.boolexpr_Array_getitem(self._cdata, index))

    def __setitem__(self, key, val):
        index = self._key2index(key)
        lib.boolexpr_Array_setitem(self._cdata, index, val._cdata)

    def _key2index(self, key):
        index = operator.index(key)
        if index < 0:
            index += self.__len__()
        if not 0 <= index < self.__len__():
            raise IndexError("Array index out of range")
        return index

    def _key2indices(self, key):
        if key.step is not None:
            raise ValueError("Array slice step is not supported")
        return key.indices(self.__len__())

    def __iter__(self):
        for i in range(self.__len__()):
            yield self.__getitem__(i)

    def __invert__(self):
        return Array(lib.boolexpr_Array_invert(self._cdata))

    def __or__(self, other):
        return Array(lib.boolexpr_Array_or(self._cdata, other._cdata))

    def __and__(self, other):
        return Array(lib.boolexpr_Array_and(self._cdata, other._cdata))

    def __xor__(self, other):
        return Array(lib.boolexpr_Array_xor(self._cdata, other._cdata))

    def __add__(self, other):
        return Array(lib.boolexpr_Array_plus(self._cdata, other._cdata))

    def __mul__(self, num):
        return Array(lib.boolexpr_Array_mul(self._cdata, num))

    def __rmul__(self, num):
        return Array(lib.boolexpr_Array_mul(self._cdata, num))

    def compose(self, var2bx):
        r"""
        Substitute a subset of support variables with other Boolean expressions.

        Returns a new expression: :math:`f(g_i, \ldots)`

        :math:`f_1 \: | \: x_i = f_2`
        """
        num = len(var2bx)
        c_vars = ffi.new("void * []", num)
        c_bxs = ffi.new("void * []", num)
        for i, (var, bx) in enumerate(var2bx.items()):
            c_vars[i] = _expect_var(var)._cdata
            c_bxs[i] = _expect_bx(bx)._cdata
        return Array(lib.boolexpr_Array_compose(self._cdata, num, c_vars, c_bxs))

    def restrict(self, point):
        r"""
        Substitute a subset of support variables with other Boolean expressions.

        Returns a new expression: :math:`f(g_i, \ldots)`

        :math:`f_1 \: | \: x_i = f_2`
        """
        num = len(point)
        c_vars = ffi.new("void * []", num)
        c_consts = ffi.new("void * []", num)
        for i, (var, const) in enumerate(point.items()):
            c_vars[i] = _expect_var(var)._cdata
            c_consts[i] = _expect_const(const)._cdata
        return Array(lib.boolexpr_Array_restrict(self._cdata, num, c_vars, c_consts))

    def equiv(self, other):
        """Return True if two arrays are equivalent.

        .. note:: While in practice this check can be quite fast,
                  SAT is an NP-complete problem, so some inputs will require
                  exponential runtime.
        """
        return bool(lib.boolexpr_Array_equiv(self._cdata, other._cdata))

    def zext(self, num):
        """Zero-extend this array by *num* bits."""
        return Array(lib.boolexpr_Array_zext(self._cdata, num))

    def sext(self, num):
        """Sign-extend this array by *num* bits."""
        return Array(lib.boolexpr_Array_sext(self._cdata, num))

    def or_reduce(self):
        """Reduce items of the array using the OR operator."""
        return _bx(lib.boolexpr_Array_or_reduce(self._cdata))

    def and_reduce(self):
        """Reduce items of the array using the AND operator."""
        return _bx(lib.boolexpr_Array_and_reduce(self._cdata))

    def xor_reduce(self):
        """Reduce items of the array using the XOR operator."""
        return _bx(lib.boolexpr_Array_xor_reduce(self._cdata))

    def lsh(self, a):
        """Left shift operator"""
        cdata = lib.boolexpr_Array_lsh(self._cdata, a._cdata)
        return _ArrayPair(cdata).t

    def rsh(self, a):
        """Right shift operator"""
        cdata = lib.boolexpr_Array_rsh(self._cdata, a._cdata)
        return _ArrayPair(cdata).t

    def arsh(self, num):
        """Arithmetic right shift operator"""
        cdata = lib.boolexpr_Array_arsh(self._cdata, num)
        return _ArrayPair(cdata).t


def zeros(*dims):
    """Return a multi-dimensional array of zeros.

    The variadic *dims* input is a sequence of dimension specs.
    A dimension spec is a two-tuple: (start index, stop index).
    If a dimension is given as a single ``int``,
    it will be converted to ``(0, stop)``.

    The dimension starts at index ``start``,
    and increments by one up to, but not including, ``stop``.
    This follows the Python slice convention.

    For example, to create a 4x4 array of zeros::

       >>> zeros(2, 4)
       array([[0, 0, 0, 0],
              [0, 0, 0, 0]])
    """
    shape = _dims2shape(*dims)
    objs = [ZERO for _ in range(_volume(shape))]
    return array(objs, shape)


def ones(*dims):
    """Return a multi-dimensional array of ones.

    The variadic *dims* input is a sequence of dimension specs.
    A dimension spec is a two-tuple: (start index, stop index).
    If a dimension is given as a single ``int``,
    it will be converted to ``(0, stop)``.

    The dimension starts at index ``start``,
    and increments by one up to, but not including, ``stop``.
    This follows the Python slice convention.

    For example, to create a 4x4 array of ones::

       >>> ones(2, 4)
       array([[1, 1, 1, 1],
              [1, 1, 1, 1]])
    """
    shape = _dims2shape(*dims)
    objs = [ONE for _ in range(_volume(shape))]
    return array(objs, shape)


def logicals(*dims):
    """Return a multi-dimensional array of X's.

    The variadic *dims* input is a sequence of dimension specs.
    A dimension spec is a two-tuple: (start index, stop index).
    If a dimension is given as a single ``int``,
    it will be converted to ``(0, stop)``.

    The dimension starts at index ``start``,
    and increments by one up to, but not including, ``stop``.
    This follows the Python slice convention.

    For example, to create a 4x4 array of X's::

       >>> logicals(2, 4)
       array([[X, X, X, X],
              [X, X, X, X]])
    """
    shape = _dims2shape(*dims)
    objs = [LOGICAL for _ in range(_volume(shape))]
    return array(objs, shape)


def illogicals(*dims):
    """Return a multi-dimensional array of X's.

    The variadic *dims* input is a sequence of dimension specs.
    A dimension spec is a two-tuple: (start index, stop index).
    If a dimension is given as a single ``int``,
    it will be converted to ``(0, stop)``.

    The dimension starts at index ``start``,
    and increments by one up to, but not including, ``stop``.
    This follows the Python slice convention.

    For example, to create a 4x4 array of X's::

       >>> illogicals(2, 4)
       array([[?, ?, ?, ?],
              [?, ?, ?, ?]])
    """
    shape = _dims2shape(*dims)
    objs = [ILLOGICAL for _ in range(_volume(shape))]
    return array(objs, shape)


def uint2nda(num, length=None):
    """Convert unsigned *num* to an array of expressions.

    The *num* argument is a non-negative integer.

    If no *length* parameter is given,
    the return value will have the minimal required length.
    Otherwise, the return value will be zero-extended to match *length*.

    For example, to convert the byte 42 (binary ``0b00101010``)::

       >>> uint2nda(42, 8)
       array([0, 1, 0, 1, 0, 1, 0, 0])
    """
    if num < 0:
        raise ValueError("expected num >= 0")
    else:
        objs = _uint2objs(num, length)
        return array(objs)


def int2nda(num, length=None):
    """Convert *num* to an array of expressions.

    The *num* argument is an ``int``.
    Negative numbers will be converted using twos-complement notation.

    If no *length* parameter is given,
    the return value will have the minimal required length.
    Otherwise, the return value will be sign-extended to match *length*.

    For example, to convert the bytes 42 (binary ``0b00101010``),
    and -42 (binary ``0b11010110``)::

       >>> int2nda(42, 8)
       array([0, 1, 0, 1, 0, 1, 0, 0])
       >>> int2nda(-42, 8)
       array([0, 1, 1, 0, 1, 0, 1, 1])
    """
    if num < 0:
        req_length = clog2(abs(num)) + 1
        objs = _uint2objs(2**req_length + num)
    else:
        req_length = clog2(num + 1) + 1
        objs = _uint2objs(num, req_length)
    if length:
        if length < req_length:
            fstr = "overflow: num = {} requires length >= {}, got length = {}"
            raise ValueError(fstr.format(num, req_length, length))
        else:
            sign = objs[-1]
            objs += [sign] * (length - req_length)
    return array(objs)


def array(objs, shape=None):
    """Return an N-dimensional array of Boolean expressions.

    The *objs* argument is a nested sequence of homogeneous Boolean expressions.
    That is, both [a, b, c, d] and [[a, b], [c, d]] are valid inputs.

    The optional *shape* parameter is a tuple of dimension specs,
    which are ``(int, int)`` tuples.
    It must match the volume of *objs*.
    The shape can always be automatically determined from *objs*,
    but you can supply it to automatically reshape a flat input.
    """
    items, autoshape = _itemize(objs)
    if shape is None:
        shape = autoshape
    else:
        _check_shape(shape)
        if _volume(shape) != len(items):
            raise ValueError("expected shape volume to match items")
    num, c_bxs = _convert_args(items)
    bxa = Array(lib.boolexpr_Array_new(num, c_bxs))
    return ndarray(bxa, shape)


class ndarray: # pylint: disable=invalid-name
    """
    N-dimensional array of Boolean expressions
    """
    def __init__(self, bxa, shape):
        self._bxa = bxa
        self._shape = shape
        self._nshape = tuple(stop - start for start, stop in shape)

    @property
    def bxa(self):
        """Return the Array object."""
        return self._bxa

    @property
    def shape(self):
        """Return the Array shape."""
        return self._shape

    def reshape(self, *dims):
        """Return an equivalent array with a modified shape."""
        shape = _dims2shape(*dims)
        if _volume(shape) != self.size:
            raise ValueError("expected shape with equal volume")
        return self.__class__(self._bxa, shape)

    def __str__(self):
        pre, post = "array(", ")"
        indent = " " * len(pre) + " "
        return pre + self._str(indent) + post

    def _str(self, indent=""):
        """Helper function for __str__"""
        if self.ndim <= 1:
            return "[" + ", ".join(str(x) for x in self) + "]"
        elif self.ndim == 2:
            sep = ",\n" + indent
            # pylint: disable=protected-access
            return "[" + sep.join(x._str(indent + " ") for x in self) + "]"
        else:
            sep = ",\n\n" + indent
            # pylint: disable=protected-access
            return "[" + sep.join(x._str(indent + " ") for x in self) + "]"

    def __repr__(self):
        return self.__str__()

    def __iter__(self):
        for i in range(self._shape[0][0], self._shape[0][1]):
            yield self[i]

    def __len__(self):
        return self._shape[0][1] - self._shape[0][0]

    def __getitem__(self, key):
        parts = self._key2parts(key)
        ranges, shape, vols = self._walk_parts(parts)
        items = list()
        for coord in itertools.product(*ranges):
            index = sum(vols[i] * coord[i] for i in range(self.ndim))
            items.append(self._bxa[index])
        if shape:
            return array(items, tuple(shape))
        else:
            return items[0]

    def __setitem__(self, key, val):
        parts = self._key2parts(key)
        ranges, _, vols = self._walk_parts(parts)
        val = _expect_array(val)
        coords = list(itertools.product(*ranges))
        if val.size != len(coords):
            fstr = "expected val.size = {}, got {}"
            raise ValueError(fstr.format(len(coords), val.size))
        for coord, _val in zip(coords, val.flat):
            index = sum(vols[i] * coord[i] for i in range(self.ndim))
            self._bxa[index] = _val

    # Operators
    def __invert__(self):
        return self.__class__(~self._bxa, self._shape)

    def __or__(self, other):
        shape = self._shape if self._shape == other.shape else None
        return self.__class__(self._bxa | other.bxa, shape)

    def __and__(self, other):
        shape = self._shape if self._shape == other.shape else None
        return self.__class__(self._bxa & other.bxa, shape)

    def __xor__(self, other):
        shape = self._shape if self._shape == other.shape else None
        return self.__class__(self._bxa ^ other.bxa, shape)

    def __lshift__(self, num):
        return self.lsh(zeros(num))[0]

    def __rshift__(self, num):
        return self.rsh(zeros(num))[1]

    def __add__(self, other):
        other = _expect_array(other)
        bxa = self.bxa + other.bxa
        shape = ((0, len(bxa)), )
        if (self.ndim == other.ndim > 1
                and self._shape[1:] == other.shape[1:]
                and self._shape[0][0] == other.shape[0][0] == 0):
            shape0 = ((0, self._shape[0][1] + other.shape[0][1]), )
            shape = shape0 + self._shape[1:]
        return self.__class__(bxa, shape)

    def __radd__(self, other):
        return _expect_array(other) + self

    def __mul__(self, num):
        if num < 0:
            raise ValueError("expected multiplier to be non-negative")
        bxa = self.bxa * num
        shape = ((0, len(bxa)), )
        if self.ndim > 1 and self._shape[0][0] == 0:
            shape0 = ((0, self._shape[0][1] * num), )
            shape = shape0 + self._shape[1:]
        return self.__class__(bxa, shape)

    def __rmul__(self, num):
        return self.__mul__(num)

    @property
    def ndim(self):
        """Return the number of dimensions."""
        return len(self._shape)

    @property
    def size(self):
        """Return the size of the array.

        The *size* of a multi-dimensional array is the product of the sizes
        of its dimensions.
        """
        return len(self._bxa)

    @property
    def flat(self):
        """Return a 1D iterator over the array."""
        return iter(self._bxa)

    def compose(self, var2bx):
        """Apply the ``compose`` method to all functions.

        Returns a new array.
        """
        return self.__class__(self._bxa.compose(var2bx), self._shape)

    def restrict(self, point):
        """Apply the ``restrict`` method to all expressions.

        Returns a new array.
        """
        return self.__class__(self._bxa.restrict(point), self._shape)

    def equiv(self, other):
        """Return True if two arrays are equivalent.

        .. note:: While in practice this check can be quite fast,
                  SAT is an NP-complete problem, so some inputs will require
                  exponential runtime.
        """
        return self._bxa.equiv(other.bxa)

    def zext(self, num):
        """Zero-extend this array by *num* bits.

        Returns a new array.
        """
        shape = ((0, self.size+num), )
        return self.__class__(self._bxa.zext(num), shape)

    def sext(self, num):
        """Sign-extend this array by *num* bits.

        Returns a new array.
        """
        shape = ((0, self.size+num), )
        return self.__class__(self._bxa.sext(num), shape)

    # Reduction operators
    def or_reduce(self):
        """OR reduction operator"""
        return self._bxa.or_reduce()

    def and_reduce(self):
        """AND reduction operator"""
        return self._bxa.and_reduce()

    def xor_reduce(self):
        """XOR reduction operator"""
        return self._bxa.xor_reduce()

    # Shift operators
    def lsh(self, a):
        """Left shift the *a* array into this array.

        Returns a two-tuple (array, array)
        """
        a = _expect_array(a)
        if not 0 <= a.size <= self.size:
            fstr = "expected 0 <= a.size <= {}"
            raise ValueError(fstr.format(self.size))
        left, right = self._bxa.lsh(a.bxa)
        return (self.__class__(left, ((0, len(left)), )),
                self.__class__(right, ((0, len(right)), )))

    def rsh(self, a):
        """Right shift the *a* array into this array.

        Returns a two-tuple (array, array)
        """
        a = _expect_array(a)
        if not 0 <= a.size <= self.size:
            fstr = "expected 0 <= a.size <= {}"
            raise ValueError(fstr.format(self.size))
        left, right = self._bxa.rsh(a.bxa)
        return (self.__class__(left, ((0, len(left)), )),
                self.__class__(right, ((0, len(right)), )))

    def arsh(self, num):
        """Arithmetically right shift the array by *num* places.

        The *num* argument must be a non-negative ``int``.

        The shift-in will be the value of the most significant bit.

        Returns a two-tuple (array, array)
        """
        if not 0 <= num <= self.size:
            raise ValueError("expected 0 <= num <= {}".format(self.size))
        left, right = self._bxa.arsh(num)
        return (self.__class__(left, ((0, len(left)), )),
                self.__class__(right, ((0, len(right)), )))

    # Subroutines of __getitem__, __setitem__
    @staticmethod
    def _part(item):
        if item is Ellipsis:
            return item
        elif isinstance(item, slice):
            return item
        else:
            return operator.index(item)

    def _key2parts(self, key):
        """Convert input key to a list of index parts."""
        if isinstance(key, tuple):
            parts = [self._part(item) for item in key]
        else:
            parts = [self._part(key)]
        if len(parts) > self.ndim:
            fstr = "expected <= {0.ndim} slice dimensions, got {1}"
            raise ValueError(fstr.format(self, len(parts)))
        parts = self._fill_parts(parts)
        parts = self._norm_parts(parts)
        return parts

    def _fill_parts(self, parts):
        """Fill all '...' and ':' slice entries."""
        # Fill '...' entries with ':'
        nfill = self.ndim - len(parts)
        fparts = list()
        for part in parts:
            if part is Ellipsis:
                while nfill:
                    fparts.append(slice(None, None))
                    nfill -= 1
                fparts.append(slice(None, None))
            else:
                fparts.append(part)
        # Append ':' to the end
        for _ in range(self.ndim - len(fparts)):
            fparts.append(slice(None, None))
        return fparts

    def _norm_parts(self, parts):
        """Normalize all key parts to array shape."""
        nparts = list()
        for i, part in enumerate(parts):
            if isinstance(part, int):
                nparts.append(self._norm_int(i, part))
            elif isinstance(part, slice):
                nparts.append(self._norm_slice(i, part))
            else:
                assert False # pragma: no cover
        return nparts

    def _norm_int(self, i, part):
        if part < 0:
            npart = part + self._nshape[i]
        else:
            npart = part - self._shape[i][0]
        if not 0 <= npart < self._nshape[i]:
            raise IndexError("ndarray index out of range")
        return npart

    def _norm_slice(self, i, part):
        if part.start is not None and part.start >= 0:
            start = part.start - self._shape[i][0]
            if start < 0:
                raise IndexError("ndarray slice out of range")
        else:
            start = part.start
        if part.stop is not None and part.stop >= 0:
            stop = part.stop - self._shape[i][0]
            if stop < 0:
                raise IndexError("ndarray slice out of range")
        else:
            stop = part.stop
        if part.step is not None:
            raise ValueError("slice step is not supported")
        return slice(start, stop)

    def _walk_parts(self, parts):
        """Walk through slice parts, and get characteristic info."""
        ranges = list()
        shape = list()
        vols = list()
        for i, part in enumerate(parts):
            if isinstance(part, int):
                ranges.append(range(part, part+1))
            elif isinstance(part, slice):
                start, stop, _ = part.indices(self._nshape[i])
                ranges.append(range(start, stop))
                shape.append((self._shape[i][0] + start,
                              self._shape[i][0] + stop))
            else:
                assert False # pragma: no cover
            vols.append(reduce(operator.mul, self._nshape[i+1:], 1))
        return ranges, shape, vols


def _check_dim(dim):
    """Verify that a dimension has the right format."""
    if not 0 <= dim[0] <= dim[1]:
        raise ValueError("expected 0 <= low <= high dimensions")


def _check_shape(shape):
    """Verify that a shape has the right format."""
    if isinstance(shape, tuple):
        for dim in shape:
            if _is_dim(dim):
                _check_dim(dim)
            else:
                raise TypeError("expected shape dimension to be (int, int)")
    else:
        raise TypeError("expected shape to be tuple of (int, int)")


def _dims2shape(*dims):
    """Convert input dimensions to a shape."""
    if not dims:
        raise ValueError("expected at least one dimension spec")
    shape = list()
    for dim in dims:
        if isinstance(dim, int):
            start, stop = (0, dim)
        elif _is_dim(dim):
            _check_dim(dim)
            start, stop = dim
        else:
            raise TypeError("expected dimension to be int or (int, int)")
        shape.append((start, stop))
    return tuple(shape)


def _is_dim(obj):
    """Return True if the object is a shape dimension."""
    return (isinstance(obj, tuple) and len(obj) == 2
            and isinstance(obj[0], int) and isinstance(obj[1], int))


def _itemize(objs):
    """Recursive helper function for array."""
    isseq = [isinstance(obj, collections.Sequence) for obj in objs]
    if not any(isseq):
        return list(objs), ((0, len(objs)), )
    elif all(isseq):
        items = list()
        shape = None
        for obj in objs:
            _items, _shape = _itemize(obj)
            if shape is None:
                shape = _shape
            elif shape != _shape:
                raise ValueError("expected uniform array dimensions")
            items += _items
        shape = ((0, len(objs)), ) + shape
        return items, shape
    else:
        raise ValueError("expected uniform array dimensions")


def _uint2objs(num, length=None):
    """Convert an unsigned integer to a list of constant expressions."""
    if num == 0:
        objs = [ZERO]
    else:
        _num = num
        objs = list()
        while _num != 0:
            objs.append(ONE if _num & 1 else ZERO)
            _num >>= 1
    if length:
        if length < len(objs):
            fstr = "overflow: num = {} requires length >= {}, got length = {}"
            raise ValueError(fstr.format(num, len(objs), length))
        else:
            while len(objs) < length:
                objs.append(ZERO)
    return objs


def _volume(shape):
    """Return the volume of a shape."""
    prod = 1
    for start, stop in shape:
        prod *= stop - start
    return prod
