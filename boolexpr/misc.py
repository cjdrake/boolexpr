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


import functools
import itertools
import operator

from .util import clog2
from .util import iter_space
from .wrap import array
from .wrap import not_
from .wrap import or_
from .wrap import and_
from .wrap import _expect_array


def nhot(n, *args):
    """
    Return a CNF expression that means
    "exactly N input functions are true".
    """
    if not 0 <= n <= len(args):
        fstr = "expected 0 <= n <= {}, got {}"
        raise ValueError(fstr.format(len(args), n))
    clauses = list()
    for xs in itertools.combinations(args, n+1):
        clauses.append(or_(*[not_(x) for x in xs]))
    for xs in itertools.combinations(args, (len(args)+1)-n):
        clauses.append(or_(*xs))
    return and_(*clauses)


def majority(*args):
    """
    Return a CNF expression that means
    "the majority of input functions are true".
    """
    clauses = list()
    for xs in itertools.combinations(args, (len(args) + 1) // 2):
        clauses.append(or_(*xs))
    return and_(*clauses)


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


def mux(a, sel):
    """
    Return an expression that multiplexes an input array over a select array.
    """
    a = _expect_array(a)
    sel = _expect_array(sel)
    if sel.size < clog2(a.size):
        fstr = "expected at least {} select bits, got {}"
        raise ValueError(fstr.format(clog2(a.size), sel.size))
    terms = (tuple(sel[i] if vertex[i] else ~sel[i] for i in range(sel.size))
             for vertex in iter_space(sel.size))
    return or_(*[and_(x, *term) for (x, term) in zip(a.flat, terms)])


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


def cat(*xs):
    """Concatenate a sequence of expressions."""
    return functools.reduce(operator.add, xs, array([]))
