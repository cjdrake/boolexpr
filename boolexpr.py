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
    """
    def __init__(self):
        self._ctx = lib.boolexpr_Context()

    def get_var(self, name):
        bx = lib.boolexpr_Context_get_var(self._ctx, name.encode('ascii'))
        return BoolExpr(bx)


class BoolExpr:
    """
    """
    def __init__(self, bx):
        self._bx = bx

    @property
    def bx(self):
        return self._bx

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        b = ffi.string(lib.boolexpr_BoolExpr_to_string(self.bx))
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
        return lib.boolexpr_BoolExpr_kind(self._bx)

    @property
    def depth(self):
        return lib.boolexpr_BoolExpr_depth(self._bx)


ZERO = BoolExpr(lib.boolexpr_zero())
ONE = BoolExpr(lib.boolexpr_one())

_CONSTS = [ZERO, ONE]


def _convert_args(args):
    n = len(args)
    _args = ffi.new("void const * [" + str(n) + "]")
    for i, arg in enumerate(args):
        if arg in {False, True}:
            _args[i] = _CONSTS[arg].bx
        elif isinstance(arg, BoolExpr):
            _args[i] = arg.bx
        else:
            raise TypeError("Expected bool or BoolExpr")
    return n, _args


def not_(arg):
    n, args = _convert_args((arg, ))
    return BoolExpr(lib.boolexpr_not(arg.bx))


def or_(*args):
    n, args = _convert_args(args)
    return BoolExpr(lib.boolexpr_or(n, args))


def and_(*args):
    n, args = _convert_args(args)
    return BoolExpr(lib.boolexpr_and(n, args))


def xor_(*args):
    n, args = _convert_args(args)
    return BoolExpr(lib.boolexpr_xor(n, args))


def eq(*args):
    n, args = _convert_args(args)
    return BoolExpr(lib.boolexpr_eq(n, args))


def impl(p, q):
    _, args = _convert_args((p, q))
    return BoolExpr(lib.boolexpr_impl(args[0], args[1]))


def ite(s, d1, d0):
    _, args = _convert_args((s, d1, d0))
    return BoolExpr(lib.boolexpr_ite(args[0], args[1], args[2]))
