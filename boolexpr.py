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


from _boolexpr import ffi, lib


class Context:
    """
    A context for new Boolean variables
    """
    def __init__(self):
        self._c_ctx = lib.boolexpr_Context()

    @property
    def c_ctx(self):
        return self._c_ctx

    def get_var(self, name):
        c_bx = lib.boolexpr_Context_get_var(self._c_ctx, name.encode('ascii'))
        return _bx(c_bx)


class BoolExpr:
    """
    Base class for Boolean expressions
    """
    def __init__(self, c_bx):
        self._c_bx = c_bx

    @property
    def c_bx(self):
        return self._c_bx

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        b = ffi.string(lib.boolexpr_BoolExpr_to_string(self._c_bx))
        return b.decode('utf-8')

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
        return xor_(self, other)

    def __rxor__(self, other):
        return xor_(other, self)

    @property
    def kind(self):
        return lib.boolexpr_BoolExpr_kind(self._c_bx)

    @property
    def depth(self):
        return lib.boolexpr_BoolExpr_depth(self._c_bx)


class Atom(BoolExpr): pass
class Constant(Atom): pass
class Known(Constant): pass
class Zero(Known): pass
class One(Known): pass
class Unknown(Constant): pass
class Logical(Unknown): pass
class Illogical(Unknown): pass
class Literal(Atom): pass
class Complement(Literal): pass
class Variable(Literal): pass
class Operator(BoolExpr): pass
class LatticeOperator(Operator): pass
class Nor(Operator): pass
class Or(LatticeOperator): pass
class Nand(Operator): pass
class And(LatticeOperator): pass
class Xnor(Operator): pass
class Xor(Operator): pass
class Unequal(Operator): pass
class Equal(Operator): pass
class NotImplies(Operator): pass
class Implies(Operator): pass
class NotIfThenElse(Operator): pass
class IfThenElse(Operator): pass


_KIND2CLS = {
    0x00: Zero,
    0x01: One,
    0x04: Logical,
    0x06: Illogical,
    0x08: Complement,
    0x09: Variable,
    0x10: Nor,
    0x11: Or,
    0x12: Nand,
    0x13: And,
    0x14: Xnor,
    0x15: Xor,
    0x16: Unequal,
    0x17: Equal,
    0x18: NotImplies,
    0x19: Implies,
    0x1A: NotIfThenElse,
    0x1B: IfThenElse,
}

def _bx(cbx):
    kind = lib.boolexpr_BoolExpr_kind(cbx)
    return _KIND2CLS[kind](cbx)


def _convert_args(args):
    n = len(args)
    _args = ffi.new("void const * [" + str(n) + "]")
    for i, arg in enumerate(args):
        if arg == False:
            _args[i] = lib.boolexpr_zero()
        elif args == True:
            _args[i] = lib.boolexpr_one()
        elif isinstance(arg, BoolExpr):
            _args[i] = arg.c_bx
        else:
            raise TypeError("Expected False, True, 0, 1, or BoolExpr")
    return n, _args


def not_(arg):
    n, args = _convert_args((arg, ))
    return _bx(lib.boolexpr_not(arg.c_bx))

def or_(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_or(n, args))

def and_(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_and(n, args))

def xor_(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_xor(n, args))

def eq(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_eq(n, args))

def impl(p, q):
    _, args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl(args[0], args[1]))

def ite(s, d1, d0):
    _, args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite(args[0], args[1], args[2]))
