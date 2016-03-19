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


#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>
#include <gtest/gtest.h>

#include "boolexpr/boolexpr.h"
#include "boolexprtest.h"


using namespace boolexpr;


class SimplifyTest : public BoolExprTest {};


TEST_F(SimplifyTest, Atoms)
{
    EXPECT_EQ(_zero->simplify()->to_string(), "0");
    EXPECT_EQ(_one->simplify()->to_string(), "1");
    EXPECT_EQ(_log->simplify()->to_string(), "X");
    EXPECT_EQ(_ill->simplify()->to_string(), "?");
}


TEST_F(SimplifyTest, NorTruthTable)
{
    EXPECT_EQ(nor_s({})->to_string(), "1");

    EXPECT_EQ(nor_s({_zero})->to_string(), "1");
    EXPECT_EQ(nor_s({_one})->to_string(),  "0");
    EXPECT_EQ(nor_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(nor_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nor_s({_zero, _zero, _one})->to_string(),  "0");
    EXPECT_EQ(nor_s({_zero, _one,  _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({_zero, _one,  _one})->to_string(),  "0");
    EXPECT_EQ(nor_s({_one,  _zero, _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({_one,  _zero, _one})->to_string(),  "0");
    EXPECT_EQ(nor_s({_one,  _one,  _zero})->to_string(), "0");
    EXPECT_EQ(nor_s({_one,  _one,  _one})->to_string(),  "0");

    EXPECT_EQ(nor_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(nor_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(nor_s({_one,  xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({xs[0], _one})->to_string(),  "0");

    EXPECT_EQ(nor_s({~xs[0], ~xs[0]})->to_string(), "x_0");
    EXPECT_EQ(nor_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(nor_s({ xs[0],  xs[0]})->to_string(), "~x_0");
}


TEST_F(SimplifyTest, OrTruthTable)
{
    EXPECT_EQ(or_s({})->to_string(), "0");

    EXPECT_EQ(or_s({_zero})->to_string(), "0");
    EXPECT_EQ(or_s({_one})->to_string(),  "1");
    EXPECT_EQ(or_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(or_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(or_s({_zero, _zero, _one})->to_string(),  "1");
    EXPECT_EQ(or_s({_zero, _one,  _zero})->to_string(), "1");
    EXPECT_EQ(or_s({_zero, _one,  _one})->to_string(),  "1");
    EXPECT_EQ(or_s({_one,  _zero, _zero})->to_string(), "1");
    EXPECT_EQ(or_s({_one,  _zero, _one})->to_string(),  "1");
    EXPECT_EQ(or_s({_one,  _one,  _zero})->to_string(), "1");
    EXPECT_EQ(or_s({_one,  _one,  _one})->to_string(),  "1");

    EXPECT_EQ(or_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(or_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(or_s({_one,  xs[0]})->to_string(), "1");
    EXPECT_EQ(or_s({xs[0], _one})->to_string(),  "1");

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
    EXPECT_EQ(nand_s({_one})->to_string(),  "0");
    EXPECT_EQ(nand_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(nand_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_zero, _zero, _one})->to_string(),  "1");
    EXPECT_EQ(nand_s({_zero, _one,  _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_zero, _one,  _one})->to_string(),  "1");
    EXPECT_EQ(nand_s({_one,  _zero, _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_one,  _zero, _one})->to_string(),  "1");
    EXPECT_EQ(nand_s({_one,  _one,  _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_one,  _one,  _one})->to_string(),  "0");

    EXPECT_EQ(nand_s({_zero, xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({xs[0], _zero})->to_string(), "1");
    EXPECT_EQ(nand_s({_one,  xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(nand_s({xs[0], _one})->to_string(),  "~x_0");

    EXPECT_EQ(nand_s({~xs[0], ~xs[0]})->to_string(), "x_0");
    EXPECT_EQ(nand_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(nand_s({ xs[0],  xs[0]})->to_string(), "~x_0");
}


TEST_F(SimplifyTest, AndTruthTable)
{
    EXPECT_EQ(and_s({})->to_string(), "1");

    EXPECT_EQ(and_s({_zero})->to_string(), "0");
    EXPECT_EQ(and_s({_one})->to_string(),  "1");
    EXPECT_EQ(and_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(and_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_zero, _zero, _one})->to_string(),  "0");
    EXPECT_EQ(and_s({_zero, _one,  _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_zero, _one,  _one})->to_string(),  "0");
    EXPECT_EQ(and_s({_one,  _zero, _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_one,  _zero, _one})->to_string(),  "0");
    EXPECT_EQ(and_s({_one,  _one,  _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_one,  _one,  _one})->to_string(),  "1");

    EXPECT_EQ(and_s({_zero, xs[0]})->to_string(), "0");
    EXPECT_EQ(and_s({xs[0], _zero})->to_string(), "0");
    EXPECT_EQ(and_s({_one,  xs[0]})->to_string(), "x_0");
    EXPECT_EQ(and_s({xs[0], _one})->to_string(),  "x_0");

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
    EXPECT_EQ(xnor_s({_one})->to_string(),  "0");
    EXPECT_EQ(xnor_s({xs[0]})->to_string(), "~x_0");

    EXPECT_EQ(xnor_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(xnor_s({_zero, _zero, _one})->to_string(),  "0");
    EXPECT_EQ(xnor_s({_zero, _one,  _zero})->to_string(), "0");
    EXPECT_EQ(xnor_s({_zero, _one,  _one})->to_string(),  "1");
    EXPECT_EQ(xnor_s({_one,  _zero, _zero})->to_string(), "0");
    EXPECT_EQ(xnor_s({_one,  _zero, _one})->to_string(),  "1");
    EXPECT_EQ(xnor_s({_one,  _one,  _zero})->to_string(), "1");
    EXPECT_EQ(xnor_s({_one,  _one,  _one})->to_string(),  "0");

    EXPECT_EQ(xnor_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(xnor_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(xnor_s({_one,  xs[0]})->to_string(), "x_0");
    EXPECT_EQ(xnor_s({xs[0], _one})->to_string(),  "x_0");

    EXPECT_EQ(xnor_s({~xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(xnor_s({ xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(xnor_s({~xs[0],  xs[0]})->to_string(), "0");
    EXPECT_EQ(xnor_s({ xs[0],  xs[0]})->to_string(), "1");
}


TEST_F(SimplifyTest, XorTruthTable)
{
    EXPECT_EQ(xor_s({})->to_string(), "0");

    EXPECT_EQ(xor_s({_zero})->to_string(), "0");
    EXPECT_EQ(xor_s({_one})->to_string(),  "1");
    EXPECT_EQ(xor_s({xs[0]})->to_string(), "x_0");

    EXPECT_EQ(xor_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(xor_s({_zero, _zero, _one})->to_string(),  "1");
    EXPECT_EQ(xor_s({_zero, _one,  _zero})->to_string(), "1");
    EXPECT_EQ(xor_s({_zero, _one,  _one})->to_string(),  "0");
    EXPECT_EQ(xor_s({_one,  _zero, _zero})->to_string(), "1");
    EXPECT_EQ(xor_s({_one,  _zero, _one})->to_string(),  "0");
    EXPECT_EQ(xor_s({_one,  _one,  _zero})->to_string(), "0");
    EXPECT_EQ(xor_s({_one,  _one,  _one})->to_string(),  "1");

    EXPECT_EQ(xor_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(xor_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(xor_s({_one,  xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(xor_s({xs[0], _one})->to_string(),  "~x_0");

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
    EXPECT_EQ(neq_s({_one})->to_string(),  "0");
    EXPECT_EQ(neq_s({xs[0]})->to_string(), "0");

    EXPECT_EQ(neq_s({_zero, _zero, _zero})->to_string(), "0");
    EXPECT_EQ(neq_s({_zero, _zero, _one})->to_string(),  "1");
    EXPECT_EQ(neq_s({_zero, _one,  _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({_zero, _one,  _one})->to_string(),  "1");
    EXPECT_EQ(neq_s({_one,  _zero, _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({_one,  _zero, _one})->to_string(),  "1");
    EXPECT_EQ(neq_s({_one,  _one,  _zero})->to_string(), "1");
    EXPECT_EQ(neq_s({_one,  _one,  _one})->to_string(),  "0");

    EXPECT_EQ(neq_s({_zero, xs[0]})->to_string(), "x_0");
    EXPECT_EQ(neq_s({xs[0], _zero})->to_string(), "x_0");
    EXPECT_EQ(neq_s({_one,  xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(neq_s({xs[0], _one})->to_string(),  "~x_0");

    EXPECT_EQ(neq_s({~xs[0], ~xs[0]})->to_string(), "0");
    EXPECT_EQ(neq_s({ xs[0], ~xs[0]})->to_string(), "1");
    EXPECT_EQ(neq_s({~xs[0],  xs[0]})->to_string(), "1");
    EXPECT_EQ(neq_s({ xs[0],  xs[0]})->to_string(), "0");
}


TEST_F(SimplifyTest, EqualTruthTable)
{
    EXPECT_EQ(eq_s({})->to_string(), "1");

    EXPECT_EQ(eq_s({_zero})->to_string(), "1");
    EXPECT_EQ(eq_s({_one})->to_string(),  "1");
    EXPECT_EQ(eq_s({xs[0]})->to_string(), "1");

    EXPECT_EQ(eq_s({_zero, _zero, _zero})->to_string(), "1");
    EXPECT_EQ(eq_s({_zero, _zero, _one})->to_string(),  "0");
    EXPECT_EQ(eq_s({_zero, _one,  _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({_zero, _one,  _one})->to_string(),  "0");
    EXPECT_EQ(eq_s({_one,  _zero, _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({_one,  _zero, _one})->to_string(),  "0");
    EXPECT_EQ(eq_s({_one,  _one,  _zero})->to_string(), "0");
    EXPECT_EQ(eq_s({_one,  _one,  _one})->to_string(),  "1");

    EXPECT_EQ(eq_s({_zero, xs[0]})->to_string(), "~x_0");
    EXPECT_EQ(eq_s({xs[0], _zero})->to_string(), "~x_0");
    EXPECT_EQ(eq_s({_one,  xs[0]})->to_string(), "x_0");
    EXPECT_EQ(eq_s({xs[0], _one})->to_string(),  "x_0");

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
    EXPECT_EQ(nimpl_s(_zero, _one)->to_string(),  "0");
    EXPECT_EQ(nimpl_s(_one,  _zero)->to_string(), "1");
    EXPECT_EQ(nimpl_s(_one,  _one)->to_string(),  "0");

    EXPECT_EQ(nimpl_s(_zero, xs[0])->to_string(), "0");
    EXPECT_EQ(nimpl_s(xs[0], _zero)->to_string(), "x_0");
    EXPECT_EQ(nimpl_s(_one,  xs[0])->to_string(), "~x_0");
    EXPECT_EQ(nimpl_s(xs[0], _one)->to_string(),  "0");

    EXPECT_EQ(nimpl_s(~xs[0], ~xs[0])->to_string(), "0");
    EXPECT_EQ(nimpl_s( xs[0], ~xs[0])->to_string(), "x_0");
    EXPECT_EQ(nimpl_s(~xs[0],  xs[0])->to_string(), "~x_0");
    EXPECT_EQ(nimpl_s( xs[0],  xs[0])->to_string(), "0");
}


TEST_F(SimplifyTest, ImpliesTruthTable)
{
    EXPECT_EQ(impl_s(_zero, _zero)->to_string(), "1");
    EXPECT_EQ(impl_s(_zero, _one)->to_string(),  "1");
    EXPECT_EQ(impl_s(_one,  _zero)->to_string(), "0");
    EXPECT_EQ(impl_s(_one,  _one)->to_string(),  "1");

    EXPECT_EQ(impl_s(_zero, xs[0])->to_string(), "1");
    EXPECT_EQ(impl_s(xs[0], _zero)->to_string(), "~x_0");
    EXPECT_EQ(impl_s(_one,  xs[0])->to_string(), "x_0");
    EXPECT_EQ(impl_s(xs[0], _one)->to_string(),  "1");

    EXPECT_EQ(impl_s(~xs[0], ~xs[0])->to_string(), "1");
    EXPECT_EQ(impl_s( xs[0], ~xs[0])->to_string(), "~x_0");
    EXPECT_EQ(impl_s(~xs[0],  xs[0])->to_string(), "x_0");
    EXPECT_EQ(impl_s( xs[0],  xs[0])->to_string(), "1");

    EXPECT_EQ(impl_s(xs[0], xs[1])->to_string(), "Implies(x_0, x_1)");
}


TEST_F(SimplifyTest, NotIfThenElseTruthTable)
{
    EXPECT_EQ(nite_s(_zero, _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(_zero, _zero, _one)->to_string(),  "0");
    EXPECT_EQ(nite_s(_zero, _one,  _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(_zero, _one,  _one)->to_string(),  "0");
    EXPECT_EQ(nite_s(_one,  _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(_one,  _zero, _one)->to_string(),  "1");
    EXPECT_EQ(nite_s(_one,  _one,  _zero)->to_string(), "0");
    EXPECT_EQ(nite_s(_one,  _one,  _one)->to_string(),  "0");

    EXPECT_EQ(nite_s(xs[0], _zero, _zero)->to_string(), "1");
    EXPECT_EQ(nite_s(xs[0], _zero, _one)->to_string(), "x_0");
    EXPECT_EQ(nite_s(xs[0], _one, _zero)->to_string(), "~x_0");
    EXPECT_EQ(nite_s(xs[0], _one, _one)->to_string(), "0");
    EXPECT_EQ(nite_s(xs[0], xs[1], xs[1])->to_string(), "~x_1");

    EXPECT_TRUE(nite_s(xs[0], _zero, xs[1])->equiv(~(~xs[0] & xs[1])));
    EXPECT_TRUE(nite_s(xs[0], _one,  xs[1])->equiv(~( xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1], _zero)->equiv(~( xs[0] & xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1],  _one)->equiv(~(~xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[0], xs[1])->equiv(~( xs[0] | xs[1])));
    EXPECT_TRUE(nite_s(xs[0], xs[1], xs[0])->equiv(~( xs[0] & xs[1])));
}


TEST_F(SimplifyTest, IfThenElseTruthTable)
{
    EXPECT_EQ(ite_s(_zero, _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(_zero, _zero, _one)->to_string(),  "1");
    EXPECT_EQ(ite_s(_zero, _one,  _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(_zero, _one,  _one)->to_string(),  "1");
    EXPECT_EQ(ite_s(_one,  _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(_one,  _zero, _one)->to_string(),  "0");
    EXPECT_EQ(ite_s(_one,  _one,  _zero)->to_string(), "1");
    EXPECT_EQ(ite_s(_one,  _one,  _one)->to_string(),  "1");

    EXPECT_EQ(ite_s(xs[0], _zero, _zero)->to_string(), "0");
    EXPECT_EQ(ite_s(xs[0], _zero, _one)->to_string(), "~x_0");
    EXPECT_EQ(ite_s(xs[0], _one, _zero)->to_string(), "x_0");
    EXPECT_EQ(ite_s(xs[0], _one, _one)->to_string(), "1");
    EXPECT_EQ(ite_s(xs[0], xs[1], xs[1])->to_string(), "x_1");

    EXPECT_TRUE(ite_s(xs[0], _zero, xs[1])->equiv(~xs[0] & xs[1]));
    EXPECT_TRUE(ite_s(xs[0], _one,  xs[1])->equiv( xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1], _zero)->equiv( xs[0] & xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1],  _one)->equiv(~xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[0], xs[1])->equiv( xs[0] | xs[1]));
    EXPECT_TRUE(ite_s(xs[0], xs[1], xs[0])->equiv( xs[0] & xs[1]));

    EXPECT_EQ(ite_s(xs[0], xs[1], xs[2])->to_string(), "IfThenElse(x_0, x_1, x_2)");
}


TEST_F(SimplifyTest, Unknowns)
{
    auto y0 = xs[0] | _log;
    EXPECT_EQ(y0->simplify()->to_string(), "X");

    auto y1 = _log | xs[0];
    EXPECT_EQ(y1->simplify()->to_string(), "X");

    auto y2 = xs[0] | _log | _one;
    EXPECT_EQ(y2->simplify()->to_string(), "1");

    auto y3 = xs[0] | _one | _log;
    EXPECT_EQ(y3->simplify()->to_string(), "1");

    auto y4 = _log | xs[0] | _one;
    EXPECT_EQ(y4->simplify()->to_string(), "1");

    auto y5 = _log | _one | xs[0];
    EXPECT_EQ(y5->simplify()->to_string(), "1");

    auto y6 = _one | xs[0] | _log;
    EXPECT_EQ(y6->simplify()->to_string(), "1");

    auto y7 = _one | _log | xs[0];
    EXPECT_EQ(y7->simplify()->to_string(), "1");

    auto y8 = xs[0] | _log | _one | _ill;
    EXPECT_EQ(y8->simplify()->to_string(), "?");

    auto y9 = _log | xs[0] | _one | _ill;
    EXPECT_EQ(y9->simplify()->to_string(), "?");

    auto y10 = _log | _one | xs[0] | _ill;
    EXPECT_EQ(y10->simplify()->to_string(), "?");

    auto y11 = _log | _one | _ill | xs[0];
    EXPECT_EQ(y11->simplify()->to_string(), "?");
}
