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

Use CFFI to interface the boolexpr C++ library to Python.
"""


import itertools
from enum import Enum

from _boolexpr import ffi, lib


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


class BoolExpr:
    """
    Base class for Boolean expressions
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_BoolExpr_del(self._cdata)

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
        return not_(self)

    def __or__(self, other):
        return or_(self, other)

    def __ror__(self, other):
        return or_(other, self)

    def __and__(self, other):
        return and_(self, other)

    def __rand__(self, other):
        return and_(other, self)

    def __xor__(self, other):
        return xor(self, other)

    def __rxor__(self, other):
        return xor(other, self)

    @property
    def kind(self):
        """Return the Kind code."""
        return Kind(lib.boolexpr_BoolExpr_kind(self._cdata))

    def depth(self):
        """Return the expression depth."""
        return lib.boolexpr_BoolExpr_depth(self._cdata)

    def size(self):
        """Return the expression size."""
        return lib.boolexpr_BoolExpr_size(self._cdata)

    def atom_count(self):
        """Return the count of atoms in the expression."""
        return lib.boolexpr_BoolExpr_atom_count(self._cdata)

    def op_count(self):
        """Return the count of operators in the expression."""
        return lib.boolexpr_BoolExpr_op_count(self._cdata)

    def is_cnf(self):
        """Return True if the expression is in conjunctive normal form (CNF)."""
        return bool(lib.boolexpr_BoolExpr_is_cnf(self._cdata))

    def is_dnf(self):
        """Return True if the expression is in disjunctive normal form (DNF)."""
        return bool(lib.boolexpr_BoolExpr_is_dnf(self._cdata))

    def pushdown_not(self):
        """Return an expression with all Not bubbles pushed down to the atoms."""
        return _bx(lib.boolexpr_BoolExpr_pushdown_not(self._cdata))

    def simplify(self):
        """Return a simplified expression."""
        return _bx(lib.boolexpr_BoolExpr_simplify(self._cdata))

    def to_binop(self):
        """Return an expression that uses only binary operators."""
        return _bx(lib.boolexpr_BoolExpr_to_binop(self._cdata))

    def to_latop(self):
        """Return an expression that consists of Or, And, and Literals."""
        return _bx(lib.boolexpr_BoolExpr_to_latop(self._cdata))

    def tseytin(self, ctx, auxvarname="a"):
        """Return the Tseytin transformation."""
        name = auxvarname.encode("ascii")
        return _bx(lib.boolexpr_BoolExpr_tseytin(self._cdata, ctx.cdata, name))

    def compose(self, var2bx):
        """Substitute a subset of support variables with other Boolean functions."""
        num = len(var2bx)
        vars_ = ffi.new("void * []", num)
        bxs = ffi.new("void * []", num)
        for i, (var, bx) in enumerate(var2bx.items()):
            vars_[i] = _expect_var(var).cdata
            bxs[i] = _expect_bx(bx).cdata
        return _bx(lib.boolexpr_BoolExpr_compose(self._cdata, num, vars_, bxs))

    def restrict(self, point):
        """Restrict a subset of support variables to {0, 1}."""
        num = len(point)
        vars_ = ffi.new("void * []", num)
        consts = ffi.new("void * []", num)
        for i, (var, const) in enumerate(point.items()):
            vars_[i] = _expect_var(var).cdata
            consts[i] = _expect_const(const).cdata
        return _bx(lib.boolexpr_BoolExpr_restrict(self._cdata, num, vars_, consts))

    def sat(self):
        """Return a tuple (is_sat, point).

        The is_sat value is True if the expression is satisfiable.
        If the expression is not satisfiable, the point will be None.
        Otherwise, it will return a {Variable: Constant} mapping of a
        satisfying input point.
        """
        soln = lib.boolexpr_BoolExpr_sat(self._cdata)
        try:
            is_sat = bool(lib.boolexpr_Soln_first(soln))
            if not is_sat:
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
        """Convert the expression to negation normal form (NNF)."""
        return _bx(lib.boolexpr_BoolExpr_to_nnf(self._cdata))

    def equiv(self, other):
        """Return True if the two expressions are formally equivalent.

        NOTE: While in practice this check can be quite fast,
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

class One(Known):
    """Boolean One"""
    def __bool__(self):
        return True

    def __int__(self):
        return 1


class Unknown(Constant):
    """Boolean Unknown Constant"""

class Logical(Unknown):
    """Boolean Logical Unknown"""

class Illogical(Unknown):
    """Boolean Illogical Unknown"""


class Literal(Atom):
    """Boolean Literal Atom"""

    @property
    def ctx(self):
        return lib.boolexpr_Literal_ctx(self._cdata);

    @property
    def id(self):
        return lib.boolexpr_Literal_id(self._cdata);


class Complement(Literal):
    """Boolean Complement"""

class Variable(Literal):
    """Boolean Variable"""


class Operator(BoolExpr):
    """Boolean Operator"""

    @property
    def simple(self):
        """Return True if the operator is simple."""
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
        """Return True if the operator is a clause."""
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

def _bx(cbx):
    kind = lib.boolexpr_BoolExpr_kind(cbx)
    if kind in _KIND2CONST:
        lib.boolexpr_BoolExpr_del(cbx)
        return _KIND2CONST[kind]
    if kind in _KIND2LIT:
        k0 = int(ffi.cast("uintptr_t", lib.boolexpr_Literal_ctx(cbx)))
        k1 = lib.boolexpr_Literal_id(cbx)
        try:
            lit = _LITS[(k0, k1)]
        except KeyError:
            lit = _LITS[(k0, k1)] = _KIND2LIT[kind](cbx)
        else:
            lib.boolexpr_BoolExpr_del(cbx)
        return lit
    return _KIND2OTHER[kind](cbx)


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

def impl(p, q):
    """Boolean Implies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl(c_args[0], c_args[1]))

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

def impl_s(p, q):
    """Simplifying Boolean Implies operator."""
    _, c_args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl_s(c_args[0], c_args[1]))

def ite_s(s, d1, d0):
    """Simplifying Boolean IfThenElse operator."""
    _, c_args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite_s(c_args[0], c_args[1], c_args[2]))


def onehot0(*args, conj=True):
    """
    Return an expression that means
    "at most one input function is true".

    If *conj* is ``True``, return a CNF.
    Otherwise, return a DNF.
    """
    terms = list()
    if conj:
        for x_0, x_1 in itertools.combinations(args, 2):
            terms.append(or_(not_(x_0), not_(x_1)))
        return and_(*terms)
    else:
        for xs in itertools.combinations(args, len(args) - 1):
            terms.append(and_(*[not_(x) for x in xs]))
        return or_(*terms)


def onehot(*args, conj=True):
    """
    Return an expression that means
    "exactly one input function is true".

    If *conj* is ``True``, return a CNF.
    Otherwise, return a DNF.
    """
    terms = list()
    if conj:
        for x_0, x_1 in itertools.combinations(args, 2):
            terms.append(or_(not_(x_0), not_(x_1)))
        terms.append(or_(*args))
        return and_(*terms)
    else:
        for i, x_i in enumerate(args):
            zeros = [not_(arg) for arg in args[:i] + args[i+1:]]
            terms.append(and_(x_i, *zeros))
        return or_(*terms)


def majority(*args, conj=False):
    """
    Return an expression that means
    "the majority of input functions are true".

    If *conj* is ``True``, return a CNF.
    Otherwise, return a DNF.
    """
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
