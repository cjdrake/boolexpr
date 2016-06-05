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
Miscellaneous features not implemented in C++ API
"""


import itertools

from .util import clog2
from .wrap import BoolExpr
from .wrap import iter_points
from .wrap import not_
from .wrap import or_
from .wrap import and_


def nhot(n, *args):
    """
    Return a CNF expression that means
    "exactly N input functions are true".
    """
    if not 0 <= n <= len(args):
        fstr = "expected 0 <= n <= {}, got {}"
        raise ValueError(fstr.format(len(args), n))
    terms = list()
    for xs in itertools.combinations(args, n+1):
        terms.append(or_(*[not_(x) for x in xs]))
    for xs in itertools.combinations(args, (len(args)+1)-n):
        terms.append(or_(*xs))
    return and_(*terms)


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


def mux(args, sel):
    """
    Return an expression that multiplexes a sequence of input functions over a
    sequence of select functions.
    """
    # convert mux([a, b], x) to mux([a, b], [x])
    if isinstance(sel, BoolExpr):
        sel = [BoolExpr]
    if len(sel) < clog2(len(args)):
        fstr = "expected at least {} select bits, got {}"
        raise ValueError(fstr.format(clog2(len(args)), len(sel)))
    gen = (tuple(v if val else ~v for v, val in point.items())
           for point in iter_points(sel))
    return or_(*[and_(arg, *next(gen)) for arg in args])


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
