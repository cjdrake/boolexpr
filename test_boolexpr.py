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

import boolexpr


class BoolExprTest(unittest.TestCase):

    def setUp(self):
        self.ctx = boolexpr.Context()
        self.xs = [self.ctx.get_var("x_" + str(i)) for i in range(64)]

    def tearDown(self):
        pass

    def test_basic(self):
        f = ~self.xs[0] | self.xs[1] & ~self.xs[2] ^ self.xs[3]
        self.assertEqual(str(f), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))")
        self.assertEqual(f.depth(), 3)
        self.assertEqual(f.size(), 7)
        self.assertEqual(f.atom_count(), 4)
        self.assertEqual(f.op_count(), 3)
        self.assertFalse(f.is_cnf())
        self.assertFalse(f.is_dnf())

    def test_equiv(self):
        f = self.xs[0] ^ self.xs[1]
        g = ~self.xs[0] & self.xs[1] | self.xs[0] & ~self.xs[1]
        self.assertTrue(f.equiv(g))


if __name__ == "__main__":
    unittest.main()
