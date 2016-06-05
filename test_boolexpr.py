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


import unittest

from boolexpr import *


# It's convenient to just reuse several global variables
ctx = Context()
xs = [ctx.get_var("x_" + str(i)) for i in range(64)]


class ContextTest(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_basic(self):
        """Context constructor/destructor"""
        ctx = Context()
        del ctx


class BoolExprTest(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_basic(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        self.assertEqual(str(f), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))")
        self.assertEqual(str(f), repr(f))
        self.assertEqual(f.depth(), 3)
        self.assertEqual(f.size(), 7)
        self.assertFalse(f.is_cnf())
        self.assertFalse(f.is_dnf())

        self.assertFalse(bool(ZERO))
        self.assertTrue(bool(ONE))
        self.assertEqual(int(ZERO), 0)
        self.assertEqual(int(ONE), 1)

    def test_ast(self):
        """BoolExpr to_ast/from_ast methods"""
        from boolexpr._boolexpr import ffi
        p = int(ffi.cast("uintptr_t", ctx._cdata))

        self.assertEqual(ZERO.to_ast(), (BoolExpr.Kind.zero, ))
        self.assertEqual(ONE.to_ast(), (BoolExpr.Kind.one, ))
        self.assertEqual(LOGICAL.to_ast(), (BoolExpr.Kind.log, ))
        self.assertEqual(ILLOGICAL.to_ast(), (BoolExpr.Kind.ill, ))

        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        ast = (BoolExpr.Kind.or_,
                  (BoolExpr.Kind.comp, p, "x_0"),
                  (BoolExpr.Kind.xor,
                      (BoolExpr.Kind.and_,
                          (BoolExpr.Kind.var, p, "x_1"),
                          (BoolExpr.Kind.comp, p, "x_2")),
                      (BoolExpr.Kind.var, p, "x_3")))

        self.assertEqual(f.to_ast(), ast)
        g = BoolExpr.from_ast(ast)
        self.assertTrue(f.equiv(g))

    def test_errors(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]

        # Expected obj to be a BoolExpr
        with self.assertRaises(TypeError):
            xs[0] | 42

        # Expected obj to be a Constant
        with self.assertRaises(TypeError):
            f.restrict({xs[0]: 42})

        # Expected obj to be a Variable
        with self.assertRaises(TypeError):
            f.compose({xs[0] & xs[1]: False})

    def test_ops1(self):
        self.assertEqual(not_(xs[0]).kind, BoolExpr.Kind.comp)

        self.assertEqual(nor(*xs[:4]).kind, BoolExpr.Kind.nor)
        self.assertEqual(or_(*xs[:4]).kind, BoolExpr.Kind.or_)
        self.assertEqual(nand(*xs[:4]).kind, BoolExpr.Kind.nand)
        self.assertEqual(and_(*xs[:4]).kind, BoolExpr.Kind.and_)
        self.assertEqual(xnor(*xs[:4]).kind, BoolExpr.Kind.xnor)
        self.assertEqual(xor(*xs[:4]).kind, BoolExpr.Kind.xor)
        self.assertEqual(neq(*xs[:4]).kind, BoolExpr.Kind.neq)
        self.assertEqual(eq(*xs[:4]).kind, BoolExpr.Kind.eq)
        self.assertEqual(nimpl(xs[0], xs[1]).kind, BoolExpr.Kind.nimpl)
        self.assertEqual(impl(xs[0], xs[1]).kind, BoolExpr.Kind.impl)
        self.assertEqual(nite(xs[0], xs[1], xs[2]).kind, BoolExpr.Kind.nite)
        self.assertEqual(ite(xs[0], xs[1], xs[2]).kind, BoolExpr.Kind.ite)

        self.assertEqual(nor_s(*xs[:4]).kind, BoolExpr.Kind.nor)
        self.assertEqual(or_s(*xs[:4]).kind, BoolExpr.Kind.or_)
        self.assertEqual(nand_s(*xs[:4]).kind, BoolExpr.Kind.nand)
        self.assertEqual(and_s(*xs[:4]).kind, BoolExpr.Kind.and_)
        self.assertEqual(xnor_s(*xs[:4]).kind, BoolExpr.Kind.xnor)
        self.assertEqual(xor_s(*xs[:4]).kind, BoolExpr.Kind.xor)
        self.assertEqual(neq_s(*xs[:4]).kind, BoolExpr.Kind.neq)
        self.assertEqual(eq_s(*xs[:4]).kind, BoolExpr.Kind.eq)
        self.assertEqual(nimpl_s(xs[0], xs[1]).kind, BoolExpr.Kind.nimpl)
        self.assertEqual(impl_s(xs[0], xs[1]).kind, BoolExpr.Kind.impl)
        self.assertEqual(nite_s(xs[0], xs[1], xs[2]).kind, BoolExpr.Kind.nite)
        self.assertEqual(ite_s(xs[0], xs[1], xs[2]).kind, BoolExpr.Kind.ite)

    def test_ops2(self):
        self.assertEqual(str(xs[0] | 'X'), "Or(x_0, X)")
        self.assertEqual(str('?' | xs[0]), "Or(?, x_0)")
        self.assertEqual(str(xs[0] & False), "And(x_0, 0)")
        self.assertEqual(str(True & xs[0]), "And(1, x_0)")
        self.assertEqual(str(xs[0] ^ 0), "Xor(x_0, 0)")
        self.assertEqual(str(1 ^ xs[0]), "Xor(1, x_0)")

    def test_ops3(self):
        f0 = onehot0(*xs[:8])
        self.assertEqual(f0.kind, BoolExpr.Kind.and_)
        self.assertEqual(len(list(f0.iter_sat())), 9)

        f2 = onehot(*xs[:8])
        self.assertEqual(f2.kind, BoolExpr.Kind.and_)
        self.assertEqual(len(list(f2.iter_sat())), 8)

        f4 = majority(*xs[:8], conj=False)
        self.assertEqual(f4.kind, BoolExpr.Kind.or_)
        f5 = majority(*xs[:8], conj=True)
        self.assertEqual(f5.kind, BoolExpr.Kind.and_)
        self.assertTrue(f4.equiv(f5))

        f6 = achilles_heel(*xs[:8])
        self.assertEqual(f6.kind, BoolExpr.Kind.and_)

        with self.assertRaises(ValueError):
            achilles_heel(*xs[:7])

    def test_to_posop(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_posop()
        self.assertEqual(str(g), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))")
        self.assertTrue(f.equiv(g))

    def test_simplify(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        self.assertFalse(f.simple)
        g = f.simplify()
        self.assertTrue(g.simple)
        self.assertTrue(f.equiv(g))

    def test_to_binop(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_binop()
        self.assertEqual(str(g), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))")
        self.assertTrue(f.equiv(g))

    def test_to_latop(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_latop()
        self.assertEqual(str(g), "Or(~x_0, Or(And(Nand(x_1, ~x_2), x_3), And(And(x_1, ~x_2), ~x_3)))")
        self.assertTrue(f.equiv(g))

    def test_tseytin(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.tseytin(ctx)
        self.assertEqual(g.depth(), 2)
        self.assertEqual(g.kind, BoolExpr.Kind.and_)

    def test_compose(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.compose({xs[0]: xs[4], xs[1]: xs[5]})
        self.assertEqual(str(g), "Or(~x_4, Xor(And(x_5, ~x_2), x_3))")

    def test_restrict(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.restrict({xs[0]: False, xs[1]: 1, xs[2]: 'X', xs[3]: '?',
                        xs[4]: 0, xs[5]: True, xs[6]: 'x', xs[7]: '?'})
        self.assertEqual(str(g), "?")

    def test_equiv(self):
        f = xs[0] ^ xs[1]
        g = ~xs[0] & xs[1] | xs[0] & ~xs[1]
        self.assertTrue(f.equiv(g))

    def test_sat(self):
        f = ~xs[0] & xs[0]
        fst, snd = f.sat()
        self.assertFalse(fst)
        self.assertIsNone(snd)
        g = xs[0] & xs[1]
        fst, snd = g.sat()
        self.assertTrue(fst)
        observed = {(str(k), str(v)) for k, v in snd.items()}
        expected = {('x_1', '1'), ('x_0', '1')}
        self.assertEqual(observed, expected)

    def test_iter_sat(self):
        f = onehot(*xs[:8])
        self.assertEqual(len(list(f.iter_sat())), 8)

    def test_cnf(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_cnf()
        self.assertEqual(g.depth(), 2)
        self.assertEqual(g.kind, BoolExpr.Kind.and_)

    def test_dnf(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_dnf()
        self.assertTrue(g.depth(), 2)
        self.assertEqual(g.kind, BoolExpr.Kind.or_)

    def test_nnf(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_nnf()
        NNF_KINDS = {BoolExpr.Kind.or_, BoolExpr.Kind.and_,
                     BoolExpr.Kind.comp, BoolExpr.Kind.var}
        for bx in g.iter_dfs():
            self.assertTrue(bx.kind in NNF_KINDS)

    def test_support(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        s = f.support()
        self.assertEqual(len(s), 4)
        observed = set(str(x) for x in s)
        expected = {"x_0", "x_1", "x_2", "x_3"}
        self.assertEqual(observed, expected)

    def test_cofactors(self):
        f = majority(*xs[:3])
        self.assertTrue(f.smoothing(xs[0]).equiv(xs[1]|xs[2]))
        self.assertTrue(f.smoothing([xs[0]]).equiv(xs[1]|xs[2]))
        self.assertTrue(exists(xs[0], f).equiv(xs[1]|xs[2]))
        self.assertTrue(f.consensus(xs[0]).equiv(xs[1]&xs[2]))
        self.assertTrue(f.consensus([xs[0]]).equiv(xs[1]&xs[2]))
        self.assertTrue(forall(xs[0], f).equiv(xs[1]&xs[2]))
        self.assertTrue(f.derivative(xs[0]).equiv(xs[1]^xs[2]))
        self.assertTrue(f.derivative([xs[0]]).equiv(xs[1]^xs[2]))
        left, right = f.iter_cfs(xs[0])
        self.assertTrue(left.equiv(xs[1] & xs[2]))
        self.assertTrue(right.equiv(xs[1] | xs[2]))
        self.assertEqual(list(f.iter_cfs([xs[0], xs[1], xs[2]])),
                         [ZERO, ZERO, ZERO, ONE, ZERO, ONE, ONE, ONE])

    def test_mux(self):
        f = mux(array(xs[3:7]), array([xs[0], xs[1], xs[2]]))
        self.assertTrue(f.args[0].equiv(xs[3] & ~xs[0] & ~xs[1] & ~xs[2]))
        self.assertTrue(f.args[1].equiv(xs[4] & xs[0] & ~xs[1] & ~xs[2]))
        self.assertTrue(f.args[2].equiv(xs[5] & ~xs[0] & xs[1] & ~xs[2]))
        self.assertTrue(f.args[3].equiv(xs[6] & xs[0] & xs[1] & ~xs[2]))

    def test_iter_dfs(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        observed = list(str(bx) for bx in f.iter_dfs())
        expected = [
            "~x_0",
            "x_1",
            "~x_2",
            "And(x_1, ~x_2)",
            "x_3",
            "Xor(And(x_1, ~x_2), x_3)",
            "Or(~x_0, Xor(And(x_1, ~x_2), x_3))"
        ]
        self.assertEqual(observed, expected)

    def test_iter_points(self):
        it1 = iter_points(xs[:2])
        self.assertEqual(next(it1), {xs[0]: ZERO, xs[1]: ZERO})
        self.assertEqual(next(it1), {xs[0]: ONE, xs[1]: ZERO})
        self.assertEqual(next(it1), {xs[0]: ZERO, xs[1]: ONE})
        self.assertEqual(next(it1), {xs[0]: ONE, xs[1]: ONE})
        with self.assertRaises(StopIteration):
            next(it1)

        it2 = iter_points(xs[0])
        self.assertEqual(next(it2), {xs[0]: ZERO})
        self.assertEqual(next(it2), {xs[0]: ONE})
        with self.assertRaises(StopIteration):
            next(it2)

    def test_iter_domain(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        self.assertEqual(len(list(f.iter_domain())), 2**4)

    def test_op_args(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        self.assertEqual(str(f.args[1].args[0].args[0]), "x_1")

    def test_op_is_clause(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        self.assertFalse(f.is_clause())
        g = or_(*xs[:8])
        self.assertTrue(g.is_clause())
        h = and_(*xs[:8])
        self.assertTrue(h.is_clause())

    def test_lit_eq(self):
        f = ~xs[0] | xs[1] & ~xs[2] ^ xs[3]
        g = f.to_nnf()
        # support set
        self.assertEqual(f.support(), g.support())
        # solution point
        self.assertEqual((xs[0] & xs[1]).sat(), (xs[1] & xs[0]).sat())


A = ctx.get_vars("a", 4)
B = ctx.get_vars("b", 8)
X = ctx.get_vars("x", 4, 4, 4)
Y = ctx.get_vars("x", (1,5), (3,7), (5,9))


class ArrayTest(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_basic(self):
        """ndarray basic properties"""
        self.assertEqual(X.ndim, 3)
        self.assertEqual(str(X), repr(X))
        self.assertEqual(len(X), 4)
        self.assertEqual(X.size, 4*4*4)

    def test_consts(self):
        """ndarray constant factory functions"""
        self.assertEqual(str(zeros(2, 2, 2)),
                         """\
array([[[0, 0],
        [0, 0]],

       [[0, 0],
        [0, 0]]])""")

        self.assertEqual(str(ones(2, 2, 2)),
                         """\
array([[[1, 1],
        [1, 1]],

       [[1, 1],
        [1, 1]]])""")

        self.assertEqual(str(logicals(2, 2, 2)),
                         """\
array([[[X, X],
        [X, X]],

       [[X, X],
        [X, X]]])""")

        self.assertEqual(str(illogicals(2, 2, 2)),
                         """\
array([[[?, ?],
        [?, ?]],

       [[?, ?],
        [?, ?]]])""")

    def test_numeric(self):
        """ndarray numeric factory functions"""
        self.assertEqual(str(uint2nda(42, 8)),
                         "array([0, 1, 0, 1, 0, 1, 0, 0])")
        self.assertEqual(str(int2nda(42)),
                         "array([0, 1, 0, 1, 0, 1, 0])")
        self.assertEqual(str(int2nda(42, 8)),
                         "array([0, 1, 0, 1, 0, 1, 0, 0])")
        self.assertEqual(str(int2nda(-42, 8)),
                         "array([0, 1, 1, 0, 1, 0, 1, 1])")

        self.assertEqual(str(uint2nda(0)), "array([0])")

    def test_numeric_errors(self):
        """ndarray numeric factory function errors"""
        # expected num >= 0
        with self.assertRaises(ValueError):
            uint2nda(-42)
        # overflow: num = 42 requires length >= ?, got length = 2
        with self.assertRaises(ValueError):
            uint2nda(42, 2)
        # overflow: num = 42 requires length >= ?, got length = 2
        with self.assertRaises(ValueError):
            int2nda(42, 2)

    def test_array_errors(self):
        """array() factory function errors"""
        # expected shape volume to match items
        with self.assertRaises(ValueError):
            array([[xs[0], xs[1]], [xs[2], xs[3]]], shape=((0, 3), (0, 3)))
        # expected uniform array dimensions
        with self.assertRaises(ValueError):
            array([xs[0], [xs[1], xs[2]]])
        with self.assertRaises(ValueError):
            array([[xs[0], xs[1]], [xs[2], xs[3], xs[4]]])
        # expected shape dimensions to be (int, int)
        with self.assertRaises(TypeError):
            array([xs[0], xs[1]], shape="foo")
        with self.assertRaises(TypeError):
            array([xs[0], xs[1]], shape=("foo", "bar"))

    def test_ops(self):
        """ndarray operators"""
        B = ctx.get_vars("b", 4)
        C = ctx.get_vars("c", 2, 2)
        D = ctx.get_vars("d", 2, 2)
        self.assertEqual(str(~A), "array([~a[0], ~a[1], ~a[2], ~a[3]])")
        self.assertEqual(str(A|B), "array([Or(a[0], b[0]), Or(a[1], b[1]), Or(a[2], b[2]), Or(a[3], b[3])])")
        self.assertEqual(str(A&B), "array([And(a[0], b[0]), And(a[1], b[1]), And(a[2], b[2]), And(a[3], b[3])])")
        self.assertEqual(str(A^B), "array([Xor(a[0], b[0]), Xor(a[1], b[1]), Xor(a[2], b[2]), Xor(a[3], b[3])])")
        self.assertEqual(str(A<<2), "array([0, 0, a[0], a[1]])")
        self.assertEqual(str(A>>2), "array([a[2], a[3], 0, 0])")
        left, right = A.arsh(2)
        self.assertEqual(str(left), "array([a[0], a[1]])")
        self.assertEqual(str(right), "array([a[2], a[3], a[3], a[3]])")
        self.assertEqual(str(A+B), "array([a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3]])")
        self.assertEqual(str(0+A), "array([0, a[0], a[1], a[2], a[3]])")
        self.assertEqual(str(C+D), """\
array([[c[0,0], c[0,1]],
       [c[1,0], c[1,1]],
       [d[0,0], d[0,1]],
       [d[1,0], d[1,1]]])""")
        self.assertEqual(str(2*A), "array([a[0], a[1], a[2], a[3], a[0], a[1], a[2], a[3]])")
        self.assertEqual(str(A*2), "array([a[0], a[1], a[2], a[3], a[0], a[1], a[2], a[3]])")
        self.assertEqual(str(2*C), """\
array([[c[0,0], c[0,1]],
       [c[1,0], c[1,1]],
       [c[0,0], c[0,1]],
       [c[1,0], c[1,1]]])""")

    def test_op_errors(self):
        """ndarray op errors"""
        with self.assertRaises(ValueError):
            -1 * A
        with self.assertRaises(ValueError):
            A * -5
        with self.assertRaises(ValueError):
            A << -1
        with self.assertRaises(ValueError):
            A >> -1

    def test_compose(self):
        """ndarray compose method"""
        self.assertEqual(str(A.compose({A[0]: A[1], A[1]: A[2], A[2]: A[3], A[3]: A[0]})),
                         "array([a[1], a[2], a[3], a[0]])")

    def test_restrict(self):
        """ndarray restrict method"""
        self.assertEqual(str(A.restrict({A[0]: 0, A[1]: 1, A[2]: "X", A[3]: "?"})),
                         "array([0, 1, X, ?])")

    def test_reshape(self):
        """ndarray reshape method"""
        XX = X.reshape((1, 5), (2, 6), (3, 7))
        self.assertTrue(X.equiv(XX))

    def test_reshape_errors(self):
        # expected shape with equal volume
        with self.assertRaises(ValueError):
            XX = X.reshape()
        with self.assertRaises(ValueError):
            XX = X.reshape((1, 8), (2, 6), (3, 7))
        with self.assertRaises(TypeError):
            XX = X.reshape("foo", "bar")

    def test_flat(self):
        """ndarray flat iterator"""
        self.assertEqual(len(list(X.flat)), 4*4*4)

    def test_zext(self):
        """ndarray zext method"""
        self.assertEqual(str(A.zext(2)), "array([a[0], a[1], a[2], a[3], 0, 0])")

    def test_sext(self):
        """ndarray sext method"""
        self.assertEqual(str(A.sext(2)), "array([a[0], a[1], a[2], a[3], a[3], a[3]])")

    def test_reductions(self):
        """ndarray or/and/xor reduce methods"""
        self.assertTrue(A.or_reduce().equiv(A[0]|A[1]|A[2]|A[3]))
        self.assertTrue(A.and_reduce().equiv(A[0]&A[1]&A[2]&A[3]))
        self.assertTrue(A.xor_reduce().equiv(A[0]^A[1]^A[2]^A[3]))

    def test_shift_errors(self):
        """ndarray lsh/rsh/arsh errors"""
        with self.assertRaises(ValueError):
            A.lsh(X)
        with self.assertRaises(ValueError):
            A.rsh(X)
        with self.assertRaises(ValueError):
            A.arsh(42)

    def test_index(self):
        self.assertEqual(str(Y[3,5,7]), "x[3,5,7]")
        self.assertEqual(str(Y[3,:,7]), """array([x[3,3,7], x[3,4,7], x[3,5,7], x[3,6,7]])""")
        self.assertEqual(str(Y[3,...]), """\
array([[x[3,3,5], x[3,3,6], x[3,3,7], x[3,3,8]],
       [x[3,4,5], x[3,4,6], x[3,4,7], x[3,4,8]],
       [x[3,5,5], x[3,5,6], x[3,5,7], x[3,5,8]],
       [x[3,6,5], x[3,6,6], x[3,6,7], x[3,6,8]]])""")
        self.assertEqual(str(Y[...,7]), """\
array([[x[1,3,7], x[1,4,7], x[1,5,7], x[1,6,7]],
       [x[2,3,7], x[2,4,7], x[2,5,7], x[2,6,7]],
       [x[3,3,7], x[3,4,7], x[3,5,7], x[3,6,7]],
       [x[4,3,7], x[4,4,7], x[4,5,7], x[4,6,7]]])""")
        self.assertEqual(str(Y[-1,-1,-1]), "x[4,6,8]")

    def test_index_error(self):
        with self.assertRaises(ValueError):
            Y[2:,:6,8:,:]
        with self.assertRaises(IndexError):
            Y[0:,...]
        with self.assertRaises(ValueError):
            Y[1:5:2,...]
        with self.assertRaises(IndexError):
            Y[0,...]
        with self.assertRaises(IndexError):
            Y[:0,...]

    def test_nhot(self):
        for i in range(6):
            f = nhot(i, *B)
            for pnt in f.iter_sat():
                cnt = 0
                for k, v in pnt.items():
                    cnt += bool(v)
                self.assertEqual(cnt, i)

    def test_nhot_error(self):
        with self.assertRaises(ValueError):
            nhot(-1, *B)
        with self.assertRaises(ValueError):
            nhot(9, *B)


if __name__ == "__main__":
    unittest.main()
