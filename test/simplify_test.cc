// Copyright 2016 Chris Drake
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <gtest/gtest.h>

#include "boolexpr/boolexpr.h"
#include "boolexprtest.h"


class SimplifyTest : public BoolExprTest {};


TEST_F(SimplifyTest, Atoms)
{
    EXPECT_EQ(_zero->simplify()->to_string(), "0");
    EXPECT_EQ( _one->simplify()->to_string(), "1");
    EXPECT_EQ( _log->simplify()->to_string(), "X");
    EXPECT_EQ( _ill->simplify()->to_string(), "?");
}


TEST_F(SimplifyTest, NorTruthTable)
{
    EXPECT_EQ(nor_s({})->to_string(), "1");

    EXPECT_EQ(nor_s({_zero})->to_string(), "1");
    EXPECT_EQ(nor_s({ _one})->to_string(), "0");
    EXPECT_EQ(nor_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(nor_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nor_s({_zero, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(nor_s({_zero,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({_zero,  _one,  _one})->to_string(), "0");
    EXPECT_EQ(nor_s({ _one, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({ _one, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(nor_s({ _one,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({ _one,  _one,  _one})->to_string(), "0");

    EXPECT_EQ(nor_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(nor_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(nor_s({ _one, xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({xs[0],  _one})->to_string(), "0");

    EXPECT_EQ(nor_s({~xs[0], ~xs[0]})->to_string(), "x_0");
    EXPECT_EQ(nor_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({ xs[0],  xs[0]})->to_string(), "~x_0");
}


TEST_F(SimplifyTest, OrTruthTable)
{
    EXPECT_EQ(or_s({})->to_string(), "0");

    EXPECT_EQ(or_s({_zero})->to_string(), "0");
    EXPECT_EQ(or_s({ _one})->to_string(), "1");
    EXPECT_EQ(or_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(or_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(or_s({_zero, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(or_s({_zero,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(or_s({_zero,  _one,  _one})->to_string(), "1");
    EXPECT_EQ(or_s({ _one, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(or_s({ _one, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(or_s({ _one,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(or_s({ _one,  _one,  _one})->to_string(), "1");

    EXPECT_EQ(or_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(or_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(or_s({ _one, xs[0]})->to_string(), "1");
    EXPECT_EQ(or_s({xs[0],  _one})->to_string(), "1");

    EXPECT_EQ(or_s({~xs[0], ~xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(or_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(or_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(or_s({ xs[0],  xs[0]})->to_string(), "x_0");

    // or(x0,  or(x1, x2)) <=>  or(x0, x1, x2)
    auto y0 = (xs[0] | xs[1] | xs[2])->simplify();
    EXPECT_EQ(y0->depth(), 1);
    EXPECT_TRUE(y0->equiv(or_({xs[0], xs[1], xs[2]})));
}


TEST_F(SimplifyTest, NandTruthTable)
{
    EXPECT_EQ(nand_s({})->to_string(), "0");

    EXPECT_EQ(nand_s({_zero})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one})->to_string(), "0");
    EXPECT_EQ(nand_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(nand_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_zero, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(nand_s({_zero,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_zero,  _one,  _one})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one,  _one,  _one})->to_string(), "0");

    EXPECT_EQ(nand_s({_zero, xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({xs[0], _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({ _one, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(nand_s({xs[0],  _one})->to_string(), "~x_0");

    EXPECT_EQ(nand_s({~xs[0], ~xs[0]})->to_string(), "x_0");
    EXPECT_EQ(nand_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({ xs[0],  xs[0]})->to_string(), "~x_0");
}


TEST_F(SimplifyTest, AndTruthTable)
{
    EXPECT_EQ(and_s({})->to_string(), "1");

    EXPECT_EQ(and_s({_zero})->to_string(), "0");
    EXPECT_EQ(and_s({ _one})->to_string(), "1");
    EXPECT_EQ(and_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(and_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_zero, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(and_s({_zero,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_zero,  _one,  _one})->to_string(), "0");
    EXPECT_EQ(and_s({ _one, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({ _one, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(and_s({ _one,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({ _one,  _one,  _one})->to_string(), "1");

    EXPECT_EQ(and_s({_zero, xs[0]})->to_string(), "0");
    EXPECT_EQ(and_s({xs[0], _zero})->to_string(), "0");
    EXPECT_EQ(and_s({ _one, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(and_s({xs[0],  _one})->to_string(), "x_0");

    EXPECT_EQ(and_s({~xs[0], ~xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(and_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(and_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(and_s({ xs[0],  xs[0]})->to_string(), "x_0");

    // and(x0,  and(x1, x2)) <=>  and(x0, x1, x2)
    auto y0 = (xs[0] & xs[1] & xs[2])->simplify();
    EXPECT_EQ(y0->depth(), 1);
    EXPECT_TRUE(y0->equiv(and_({xs[0], xs[1], xs[2]})));
}


TEST_F(SimplifyTest, XnorTruthTable)
{
    EXPECT_EQ(xnor_s({})->to_string(), "1");

    EXPECT_EQ(xnor_s({_zero})->to_string(), "1");
    EXPECT_EQ(xnor_s({ _one})->to_string(), "0");
    EXPECT_EQ(xnor_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(xnor_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(xnor_s({_zero, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(xnor_s({_zero,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(xnor_s({_zero,  _one,  _one})->to_string(), "1");
    EXPECT_EQ(xnor_s({ _one, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(xnor_s({ _one, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(xnor_s({ _one,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(xnor_s({ _one,  _one,  _one})->to_string(), "0");

    EXPECT_EQ(xnor_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(xnor_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(xnor_s({ _one, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(xnor_s({xs[0],  _one})->to_string(), "x_0");

    EXPECT_EQ(xnor_s({~xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(xnor_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(xnor_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(xnor_s({ xs[0],  xs[0]})->to_string(), "1");
}


TEST_F(SimplifyTest, XorTruthTable)
{
    EXPECT_EQ(xor_s({})->to_string(), "0");

    EXPECT_EQ(xor_s({_zero})->to_string(), "0");
    EXPECT_EQ(xor_s({ _one})->to_string(), "1");
    EXPECT_EQ(xor_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(xor_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(xor_s({_zero, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(xor_s({_zero,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(xor_s({_zero,  _one,  _one})->to_string(), "0");
    EXPECT_EQ(xor_s({ _one, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(xor_s({ _one, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(xor_s({ _one,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(xor_s({ _one,  _one,  _one})->to_string(), "1");

    EXPECT_EQ(xor_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(xor_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(xor_s({ _one, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(xor_s({xs[0],  _one})->to_string(), "~x_0");

    EXPECT_EQ(xor_s({~xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(xor_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(xor_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(xor_s({ xs[0],  xs[0]})->to_string(), "0");

    // xor(x0,  xor(x1, x2)) <=>  xor(x0, x1, x2)
    auto y0 = (xs[0] ^ xs[1] ^ xs[2])->simplify();
    EXPECT_EQ(y0->depth(), 1);
    EXPECT_TRUE(y0->equiv(xor_({xs[0], xs[1], xs[2]})));

    // xnor(x0,  xor(x1, x2)) <=> xnor(x0, x1, x2)
    auto y1 = xnor({xs[0], xs[1] ^ xs[2]})->simplify();
    EXPECT_EQ(y1->depth(), 1);
    EXPECT_TRUE(y1->equiv(xnor({xs[0], xs[1], xs[2]})));

    // xor(x0, xnor(x1, x2)) <=> xnor(x0, x1, x2)
    auto y2 = xor_({xs[0], xnor({xs[1], xs[2]})})->simplify();
    EXPECT_EQ(y2->depth(), 1);
    EXPECT_TRUE(y2->equiv(xnor({xs[0], xs[1], xs[2]})));

    // xnor(x0, xnor(x1, x2)) <=> xor(x0, x1, x2)
    auto y3 = xnor({xs[0], xnor({xs[1], xs[2]})})->simplify();
    EXPECT_EQ(y3->depth(), 1);
    EXPECT_TRUE(y3->equiv(xor_({xs[0], xs[1], xs[2]})));
}


TEST_F(SimplifyTest, UnequalTruthTable)
{
    EXPECT_EQ(neq_s({})->to_string(), "0");

    EXPECT_EQ(neq_s({_zero})->to_string(), "0");
    EXPECT_EQ(neq_s({ _one})->to_string(), "0");
    EXPECT_EQ(neq_s({xs[0]})->to_string(), "0");

    EXPECT_EQ(neq_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(neq_s({_zero, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(neq_s({_zero,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({_zero,  _one,  _one})->to_string(), "1");
    EXPECT_EQ(neq_s({ _one, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({ _one, _zero,  _one})->to_string(), "1");
    EXPECT_EQ(neq_s({ _one,  _one, _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({ _one,  _one,  _one})->to_string(), "0");

    EXPECT_EQ(neq_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(neq_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(neq_s({ _one, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(neq_s({xs[0],  _one})->to_string(), "~x_0");

    EXPECT_EQ(neq_s({~xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(neq_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(neq_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(neq_s({ xs[0],  xs[0]})->to_string(), "0");
}


TEST_F(SimplifyTest, EqualTruthTable)
{
    EXPECT_EQ(eq_s({})->to_string(), "1");

    EXPECT_EQ(eq_s({_zero})->to_string(), "1");
    EXPECT_EQ(eq_s({ _one})->to_string(), "1");
    EXPECT_EQ(eq_s({xs[0]})->to_string(), "1");

    EXPECT_EQ(eq_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(eq_s({_zero, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(eq_s({_zero,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({_zero,  _one,  _one})->to_string(), "0");
    EXPECT_EQ(eq_s({ _one, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({ _one, _zero,  _one})->to_string(), "0");
    EXPECT_EQ(eq_s({ _one,  _one, _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({ _one,  _one,  _one})->to_string(), "1");

    EXPECT_EQ(eq_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(eq_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(eq_s({ _one, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(eq_s({xs[0],  _one})->to_string(), "x_0");

    EXPECT_EQ(eq_s({~xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(eq_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(eq_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(eq_s({ xs[0],  xs[0]})->to_string(), "1");

    auto y0 = eq_s({xs[0], xs[1], xs[0], xs[1], xs[0]})->simplify();
    EXPECT_TRUE(y0->equiv(eq({xs[0], xs[1]})));
}


TEST_F(SimplifyTest, NotImpliesTruthTable)
{
    EXPECT_EQ(nimpl_s(_zero, _zero)->to_string(), "0");
    EXPECT_EQ(nimpl_s(_zero,  _one)->to_string(), "0");
    EXPECT_EQ(nimpl_s( _one, _zero)->to_string(), "1");
    EXPECT_EQ(nimpl_s( _one,  _one)->to_string(), "0");

    EXPECT_EQ(nimpl_s(_zero, xs[0])->to_string(), "0");
    EXPECT_EQ(nimpl_s(xs[0], _zero)->to_string(), "x_0");
    EXPECT_EQ(nimpl_s( _one, xs[0])->to_string(), "~x_0");
    EXPECT_EQ(nimpl_s(xs[0],  _one)->to_string(), "0");

    EXPECT_EQ(nimpl_s(~xs[0], ~xs[0])->to_string(), "0");
    EXPECT_EQ(nimpl_s( xs[0], ~xs[0])->to_string(), "x_0");
    EXPECT_EQ(nimpl_s(~xs[0],  xs[0])->to_string(), "~x_0");
    EXPECT_EQ(nimpl_s( xs[0],  xs[0])->to_string(), "0");
}


TEST_F(SimplifyTest, ImpliesTruthTable)
{
    EXPECT_EQ(impl_s(_zero, _zero)->to_string(), "1");
    EXPECT_EQ(impl_s(_zero,  _one)->to_string(), "1");
    EXPECT_EQ(impl_s( _one, _zero)->to_string(), "0");
    EXPECT_EQ(impl_s( _one,  _one)->to_string(), "1");

    EXPECT_EQ(impl_s(_zero, xs[0])->to_string(), "1");
    EXPECT_EQ(impl_s(xs[0], _zero)->to_string(), "~x_0");
    EXPECT_EQ(impl_s( _one, xs[0])->to_string(), "x_0");
    EXPECT_EQ(impl_s(xs[0],  _one)->to_string(), "1");

    EXPECT_EQ(impl_s(~xs[0], ~xs[0])->to_string(), "1");
    EXPECT_EQ(impl_s( xs[0], ~xs[0])->to_string(), "~x_0");
    EXPECT_EQ(impl_s(~xs[0],  xs[0])->to_string(), "x_0");
    EXPECT_EQ(impl_s( xs[0],  xs[0])->to_string(), "1");

    EXPECT_EQ(impl_s(xs[0], xs[1])->to_string(), "Implies(x_0, x_1)");
}


TEST_F(SimplifyTest, NotIfThenElseTruthTable)
{
    EXPECT_EQ(nite_s(_zero, _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(_zero, _zero,  _one)->to_string(), "0");
    EXPECT_EQ(nite_s(_zero,  _one, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(_zero,  _one,  _one)->to_string(), "0");
    EXPECT_EQ(nite_s( _one, _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s( _one, _zero,  _one)->to_string(), "1");
    EXPECT_EQ(nite_s( _one,  _one, _zero)->to_string(), "0");
    EXPECT_EQ(nite_s( _one,  _one,  _one)->to_string(), "0");

    EXPECT_EQ(nite_s(xs[0], _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(xs[0], _zero,  _one)->to_string(), "x_0");
    EXPECT_EQ(nite_s(xs[0],  _one, _zero)->to_string(), "~x_0");
    EXPECT_EQ(nite_s(xs[0],  _one,  _one)->to_string(), "0");
    EXPECT_EQ(nite_s(xs[0], xs[1], xs[1])->to_string(), "~x_1");

    EXPECT_TRUE(nite_s(xs[0], _zero, xs[1])->equiv(~(~xs[0] & xs[1])));
    EXPECT_TRUE(nite_s(xs[0],  _one, xs[1])->equiv(~( xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1], _zero)->equiv(~( xs[0] & xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1],  _one)->equiv(~(~xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[0], xs[1])->equiv(~( xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1], xs[0])->equiv(~( xs[0] & xs[1])));
}


TEST_F(SimplifyTest, IfThenElseTruthTable)
{
    EXPECT_EQ(ite_s(_zero, _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(_zero, _zero,  _one)->to_string(), "1");
    EXPECT_EQ(ite_s(_zero,  _one, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(_zero,  _one,  _one)->to_string(), "1");
    EXPECT_EQ(ite_s( _one, _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s( _one, _zero,  _one)->to_string(), "0");
    EXPECT_EQ(ite_s( _one,  _one, _zero)->to_string(), "1");
    EXPECT_EQ(ite_s( _one,  _one,  _one)->to_string(), "1");

    EXPECT_EQ(ite_s(xs[0], _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(xs[0], _zero,  _one)->to_string(), "~x_0");
    EXPECT_EQ(ite_s(xs[0],  _one, _zero)->to_string(), "x_0");
    EXPECT_EQ(ite_s(xs[0],  _one,  _one)->to_string(), "1");
    EXPECT_EQ(ite_s(xs[0], xs[1], xs[1])->to_string(), "x_1");

    EXPECT_TRUE(ite_s(xs[0], _zero, xs[1])->equiv(~xs[0] & xs[1]));
    EXPECT_TRUE(ite_s(xs[0],  _one, xs[1])->equiv( xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1], _zero)->equiv( xs[0] & xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1],  _one)->equiv(~xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[0], xs[1])->equiv( xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1], xs[0])->equiv( xs[0] & xs[1]));

    EXPECT_EQ(ite_s(xs[0], xs[1], xs[2])->to_string(), "IfThenElse(x_0, x_1, x_2)");
}


TEST_F(SimplifyTest, ImpliesUnknowns)
{
    EXPECT_EQ(impl_s(xs[0], _log)->to_string(), "X");
    EXPECT_EQ(impl_s(_log, xs[0])->to_string(), "X");

    EXPECT_EQ(impl_s(xs[0], _ill)->to_string(), "?");
    EXPECT_EQ(impl_s(_ill, xs[0])->to_string(), "?");

    EXPECT_EQ(impl_s(_log, _ill)->to_string(), "?");
    EXPECT_EQ(impl_s(_ill, _log)->to_string(), "?");
}


TEST_F(SimplifyTest, IfThenElseUnknowns)
{
    EXPECT_EQ(ite_s(_ill, xs[0], xs[1])->to_string(), "?");
    EXPECT_EQ(ite_s(xs[0], _ill, xs[1])->to_string(), "?");
    EXPECT_EQ(ite_s(xs[0], xs[1], _ill)->to_string(), "?");

    EXPECT_EQ(ite_s(xs[0], _log, xs[1])->to_string(), "X");
    EXPECT_EQ(ite_s(xs[0], xs[1], _log)->to_string(), "X");
    EXPECT_EQ(ite_s(_log, xs[0], xs[1])->to_string(), "X");
}


TEST_F(SimplifyTest, LatticeTransitions)
{
    // infimum => infimum
    auto y0 = or_({_zero, _zero});
    EXPECT_EQ(y0->simplify()->to_string(), "0");

    // infimum => basic
    auto y1 = or_({_zero, xs[0]});
    EXPECT_EQ(y1->simplify()->to_string(), "x_0");

    // infimum => islog
    auto y2 = or_({_zero, _log});
    EXPECT_EQ(y2->simplify()->to_string(), "X");

    // infimum => supremum
    auto y3 = or_({_zero, _one});
    EXPECT_EQ(y3->simplify()->to_string(), "1");

    // infimum => isill
    auto y4 = or_({_zero, _ill});
    EXPECT_EQ(y4->simplify()->to_string(), "?");

    // infimum recursion
    auto y5 = or_({_zero, or_({xs[0], xs[1]})});
    EXPECT_TRUE(y5->simplify()->equiv(or_({xs[0], xs[1]})));

    // basic => basic
    auto y6 = or_({xs[0], _zero, xs[0]});
    EXPECT_EQ(y6->simplify()->to_string(), "x_0");

    // basic => islog
    auto y7 = or_({xs[0], _log});
    EXPECT_EQ(y7->simplify()->to_string(), "X");

    // basic => supremum
    auto y8 = or_({xs[0], _one});
    EXPECT_EQ(y8->simplify()->to_string(), "1");

    // basic => isill
    auto y9 = or_({xs[0], _ill});
    EXPECT_EQ(y9->simplify()->to_string(), "?");

    // basic recursion
    auto y10 = or_({xs[0], or_({xs[0], xs[1]})});
    EXPECT_TRUE(y10->simplify()->equiv(or_({xs[0], xs[1]})));

    // islog => islog
    auto y11 = or_({_log, _zero, xs[0]});
    EXPECT_EQ(y11->simplify()->to_string(), "X");

    // islog => supremum
    auto y12 = or_({_log, _one});
    EXPECT_EQ(y12->simplify()->to_string(), "1");

    // islog => isill
    auto y13 = or_({_log, _ill});
    EXPECT_EQ(y13->simplify()->to_string(), "?");

    // islog recursion
    auto y14 = or_({_log, or_({xs[0], xs[1]})});
    EXPECT_EQ(y14->simplify()->to_string(), "X");

    // supremum => supremum
    auto y15 = or_({_one, _zero, xs[0], _log});
    EXPECT_EQ(y15->simplify()->to_string(), "1");

    // supremum => isill
    auto y16 = or_({_one, _ill});
    EXPECT_EQ(y16->simplify()->to_string(), "?");

    // supremum recursion
    auto y17 = or_({_one, or_({xs[0], xs[1]})});
    EXPECT_EQ(y17->simplify()->to_string(), "1");

    // isill => *
    auto y18 = or_({_ill, xs[0], _log, _zero, _one});
    EXPECT_EQ(y18->simplify()->to_string(), "?");
}


TEST_F(SimplifyTest, XorTransitions)
{
    // basic => basic
    auto y0 = xor_({xs[0], _zero});
    EXPECT_EQ(y0->simplify()->to_string(), "x_0");

    // basic => islog
    auto y1 = xor_({xs[0], _log});
    EXPECT_EQ(y1->simplify()->to_string(), "X");

    // basic => isill
    auto y2 = xor_({xs[0], _ill});
    EXPECT_EQ(y2->simplify()->to_string(), "?");

    // basic recursion
    auto y3 = xor_({xs[0], xor_({xs[1], xs[2]})});
    EXPECT_TRUE(y3->simplify()->equiv(xor_({xs[0], xs[1], xs[2]})));

    auto y4 = xor_({xs[0], xnor({xs[1], xs[2]})});
    EXPECT_TRUE(y4->simplify()->equiv(xnor({xs[0], xs[1], xs[2]})));

    // islog => islog
    auto y5 = xor_({_log, _zero, xs[0]});
    EXPECT_EQ(y5->simplify()->to_string(), "X");

    // islog => isill
    auto y6 = xor_({_log, _ill});
    EXPECT_EQ(y6->simplify()->to_string(), "?");

    // isill => *
    auto y7 = xor_({_ill, xs[0], _log, _zero, _one});
    EXPECT_EQ(y7->simplify()->to_string(), "?");
}


TEST_F(SimplifyTest, EqualTransitions)
{
    // basic => basic
    auto y0 = eq({xs[0], _one});
    EXPECT_EQ(y0->simplify()->to_string(), "x_0");

    // basic => islog
    auto y1 = eq({xs[0], _log});
    EXPECT_EQ(y1->simplify()->to_string(), "X");

    // basic => isill
    auto y2 = eq({xs[0], _ill});
    EXPECT_EQ(y2->simplify()->to_string(), "?");

    // islog => islog
    auto y3 = eq({_log, _zero, xs[0]});
    EXPECT_EQ(y3->simplify()->to_string(), "X");

    // islog => isill
    auto y4 = eq({_log, _ill});
    EXPECT_EQ(y4->simplify()->to_string(), "?");

    // isill => *
    auto y5 = eq({_ill, xs[0], _log, _zero, _one});
    EXPECT_EQ(y5->simplify()->to_string(), "?");
}
