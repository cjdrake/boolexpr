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
BoolExpr is an open source library for symbolic Boolean Algebra.

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

var2bx
   A dictionary of ``{Variable : BoolExpr}`` mappings.
   For example, ``{a: False, b: ~p | q}``.
"""


from .wrap import Context

from .wrap import BoolExpr
from .wrap import Atom
from .wrap import Constant
from .wrap import Known
from .wrap import Zero
from .wrap import One
from .wrap import Unknown
from .wrap import Logical
from .wrap import Illogical
from .wrap import Literal
from .wrap import Complement
from .wrap import Variable
from .wrap import Operator
from .wrap import LatticeOperator
from .wrap import Nor
from .wrap import Or
from .wrap import Nand
from .wrap import And
from .wrap import Xnor
from .wrap import Xor
from .wrap import Unequal
from .wrap import Equal
from .wrap import NotImplies
from .wrap import Implies
from .wrap import NotIfThenElse
from .wrap import IfThenElse

from .wrap import ZERO
from .wrap import ONE
from .wrap import LOGICAL
from .wrap import ILLOGICAL

from .wrap import iter_points

from .wrap import not_
from .wrap import nor
from .wrap import or_
from .wrap import nand
from .wrap import and_
from .wrap import xnor
from .wrap import xor
from .wrap import neq
from .wrap import eq
from .wrap import nimpl
from .wrap import impl
from .wrap import nite
from .wrap import ite

from .wrap import nor_s
from .wrap import or_s
from .wrap import nand_s
from .wrap import and_s
from .wrap import xnor_s
from .wrap import xor_s
from .wrap import neq_s
from .wrap import eq_s
from .wrap import nimpl_s
from .wrap import impl_s
from .wrap import nite_s
from .wrap import ite_s

from .wrap import onehot0
from .wrap import onehot

from .wrap import Array

from .wrap import zeros
from .wrap import ones
from .wrap import logicals
from .wrap import illogicals
from .wrap import uint2nda
from .wrap import int2nda
from .wrap import array

from .wrap import ndarray

from .misc import nhot
from .misc import majority
from .misc import achilles_heel
from .misc import mux
from .misc import exists
from .misc import forall


__version__ = "1.3"
