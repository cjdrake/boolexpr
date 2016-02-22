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
    EXPECT_EQ(str(simplify(_zero)), "0");
    EXPECT_EQ(str(simplify(_one)), "1");
    EXPECT_EQ(str(simplify(_logical)), "X");
    EXPECT_EQ(str(simplify(_illogical)), "?");
}


TEST_F(SimplifyTest, NorTruthTable)
{
    EXPECT_EQ(str(simplify(~or_({}))), "1");

    EXPECT_EQ(str(simplify(~or_({_zero}))), "1");
    EXPECT_EQ(str(simplify(~or_({_one}))), "0");
    EXPECT_EQ(str(simplify(~or_({xs[0]}))), "~x_0");

    EXPECT_EQ(str(simplify(~or_({_zero, _zero}))), "1");
    EXPECT_EQ(str(simplify(~or_({_zero, _one}))), "0");
    EXPECT_EQ(str(simplify(~or_({_one, _zero}))), "0");
    EXPECT_EQ(str(simplify(~or_({_one, _one}))), "0");

    EXPECT_EQ(str(simplify(~or_({_zero, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(~or_({xs[0], _zero}))), "~x_0");
    EXPECT_EQ(str(simplify(~or_({_one, xs[0]}))), "0");
    EXPECT_EQ(str(simplify(~or_({xs[0], _one}))), "0");

    EXPECT_EQ(str(simplify(~or_({xs[0], ~xs[0]}))), "0");
    EXPECT_EQ(str(simplify(~or_({~xs[0], xs[0]}))), "0");
    EXPECT_EQ(str(simplify(~or_({xs[0], xs[0]}))), "~x_0");
}


TEST_F(SimplifyTest, OrTruthTable)
{
    EXPECT_EQ(str(simplify(or_({}))), "0");

    EXPECT_EQ(str(simplify(or_({_zero}))), "0");
    EXPECT_EQ(str(simplify(or_({_one}))), "1");
    EXPECT_EQ(str(simplify(or_({xs[0]}))), "x_0");

    EXPECT_EQ(str(simplify(or_({_zero, _zero}))), "0");
    EXPECT_EQ(str(simplify(or_({_zero, _one}))), "1");
    EXPECT_EQ(str(simplify(or_({_one, _zero}))), "1");
    EXPECT_EQ(str(simplify(or_({_one, _one}))), "1");

    EXPECT_EQ(str(simplify(or_({_zero, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(or_({xs[0], _zero}))), "x_0");
    EXPECT_EQ(str(simplify(or_({_one, xs[0]}))), "1");
    EXPECT_EQ(str(simplify(or_({xs[0], _one}))), "1");

    EXPECT_EQ(str(simplify(or_({xs[0], ~xs[0]}))), "1");
    EXPECT_EQ(str(simplify(or_({~xs[0], xs[0]}))), "1");
    EXPECT_EQ(str(simplify(or_({xs[0], xs[0]}))), "x_0");
}


TEST_F(SimplifyTest, NandTruthTable)
{
    EXPECT_EQ(str(simplify(~and_({}))), "0");

    EXPECT_EQ(str(simplify(~and_({_zero}))), "1");
    EXPECT_EQ(str(simplify(~and_({_one}))), "0");
    EXPECT_EQ(str(simplify(~and_({xs[0]}))), "~x_0");

    EXPECT_EQ(str(simplify(~and_({_zero, _zero}))), "1");
    EXPECT_EQ(str(simplify(~and_({_zero, _one}))), "1");
    EXPECT_EQ(str(simplify(~and_({_one, _zero}))), "1");
    EXPECT_EQ(str(simplify(~and_({_one, _one}))), "0");

    EXPECT_EQ(str(simplify(~and_({_zero, xs[0]}))), "1");
    EXPECT_EQ(str(simplify(~and_({xs[0], _zero}))), "1");
    EXPECT_EQ(str(simplify(~and_({_one, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(~and_({xs[0], _one}))), "~x_0");

    EXPECT_EQ(str(simplify(~and_({xs[0], ~xs[0]}))), "1");
    EXPECT_EQ(str(simplify(~and_({~xs[0], xs[0]}))), "1");
    EXPECT_EQ(str(simplify(~and_({xs[0], xs[0]}))), "~x_0");
}


TEST_F(SimplifyTest, AndTruthTable)
{
    EXPECT_EQ(str(simplify(and_({}))), "1");

    EXPECT_EQ(str(simplify(and_({_zero}))), "0");
    EXPECT_EQ(str(simplify(and_({_one}))), "1");
    EXPECT_EQ(str(simplify(and_({xs[0]}))), "x_0");

    EXPECT_EQ(str(simplify(and_({_zero, _zero}))), "0");
    EXPECT_EQ(str(simplify(and_({_zero, _one}))), "0");
    EXPECT_EQ(str(simplify(and_({_one, _zero}))), "0");
    EXPECT_EQ(str(simplify(and_({_one, _one}))), "1");

    EXPECT_EQ(str(simplify(and_({_zero, xs[0]}))), "0");
    EXPECT_EQ(str(simplify(and_({xs[0], _zero}))), "0");
    EXPECT_EQ(str(simplify(and_({_one, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(and_({xs[0], _one}))), "x_0");

    EXPECT_EQ(str(simplify(and_({xs[0], ~xs[0]}))), "0");
    EXPECT_EQ(str(simplify(and_({~xs[0], xs[0]}))), "0");
    EXPECT_EQ(str(simplify(and_({xs[0], xs[0]}))), "x_0");
}


TEST_F(SimplifyTest, XnorTruthTable)
{
    EXPECT_EQ(str(simplify(~xor_({}))), "1");

    EXPECT_EQ(str(simplify(~xor_({_zero}))), "1");
    EXPECT_EQ(str(simplify(~xor_({_one}))), "0");
    EXPECT_EQ(str(simplify(~xor_({xs[0]}))), "~x_0");

    EXPECT_EQ(str(simplify(~xor_({_zero, _zero}))), "1");
    EXPECT_EQ(str(simplify(~xor_({_zero, _one}))), "0");
    EXPECT_EQ(str(simplify(~xor_({_one, _zero}))), "0");
    EXPECT_EQ(str(simplify(~xor_({_one, _one}))), "1");

    EXPECT_EQ(str(simplify(~xor_({_zero, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(~xor_({xs[0], _zero}))), "~x_0");
    EXPECT_EQ(str(simplify(~xor_({_one, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(~xor_({xs[0], _one}))), "x_0");

    EXPECT_EQ(str(simplify(~xor_({xs[0], ~xs[0]}))), "0");
    EXPECT_EQ(str(simplify(~xor_({~xs[0], xs[0]}))), "0");
    EXPECT_EQ(str(simplify(~xor_({xs[0], xs[0]}))), "1");
}


TEST_F(SimplifyTest, XorTruthTable)
{
    EXPECT_EQ(str(simplify(xor_({}))), "0");

    EXPECT_EQ(str(simplify(xor_({_zero}))), "0");
    EXPECT_EQ(str(simplify(xor_({_one}))), "1");
    EXPECT_EQ(str(simplify(xor_({xs[0]}))), "x_0");

    EXPECT_EQ(str(simplify(xor_({_zero, _zero}))), "0");
    EXPECT_EQ(str(simplify(xor_({_zero, _one}))), "1");
    EXPECT_EQ(str(simplify(xor_({_one, _zero}))), "1");
    EXPECT_EQ(str(simplify(xor_({_one, _one}))), "0");

    EXPECT_EQ(str(simplify(xor_({_zero, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(xor_({xs[0], _zero}))), "x_0");
    EXPECT_EQ(str(simplify(xor_({_one, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(xor_({xs[0], _one}))), "~x_0");

    EXPECT_EQ(str(simplify(xor_({xs[0], ~xs[0]}))), "1");
    EXPECT_EQ(str(simplify(xor_({~xs[0], xs[0]}))), "1");
    EXPECT_EQ(str(simplify(xor_({xs[0], xs[0]}))), "0");
}


TEST_F(SimplifyTest, UnequalTruthTable)
{
    EXPECT_EQ(str(simplify(~eq({}))), "0");

    EXPECT_EQ(str(simplify(~eq({_zero}))), "0");
    EXPECT_EQ(str(simplify(~eq({_one}))), "0");
    EXPECT_EQ(str(simplify(~eq({xs[0]}))), "0");

    EXPECT_EQ(str(simplify(~eq({_zero, _zero}))), "0");
    EXPECT_EQ(str(simplify(~eq({_zero, _one}))), "1");
    EXPECT_EQ(str(simplify(~eq({_one, _zero}))), "1");
    EXPECT_EQ(str(simplify(~eq({_one, _one}))), "0");

    EXPECT_EQ(str(simplify(~eq({_zero, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(~eq({xs[0], _zero}))), "x_0");
    EXPECT_EQ(str(simplify(~eq({_one, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(~eq({xs[0], _one}))), "~x_0");

    EXPECT_EQ(str(simplify(~eq({xs[0], ~xs[0]}))), "1");
    EXPECT_EQ(str(simplify(~eq({~xs[0], xs[0]}))), "1");
    EXPECT_EQ(str(simplify(~eq({xs[0], xs[0]}))), "0");
}


TEST_F(SimplifyTest, EqualTruthTable)
{
    EXPECT_EQ(str(simplify(eq({}))), "1");

    EXPECT_EQ(str(simplify(eq({_zero}))), "1");
    EXPECT_EQ(str(simplify(eq({_one}))), "1");
    EXPECT_EQ(str(simplify(eq({xs[0]}))), "1");

    EXPECT_EQ(str(simplify(eq({_zero, _zero}))), "1");
    EXPECT_EQ(str(simplify(eq({_zero, _one}))), "0");
    EXPECT_EQ(str(simplify(eq({_one, _zero}))), "0");
    EXPECT_EQ(str(simplify(eq({_one, _one}))), "1");

    EXPECT_EQ(str(simplify(eq({_zero, xs[0]}))), "~x_0");
    EXPECT_EQ(str(simplify(eq({xs[0], _zero}))), "~x_0");
    EXPECT_EQ(str(simplify(eq({_one, xs[0]}))), "x_0");
    EXPECT_EQ(str(simplify(eq({xs[0], _one}))), "x_0");

    EXPECT_EQ(str(simplify(eq({xs[0], ~xs[0]}))), "0");
    EXPECT_EQ(str(simplify(eq({~xs[0], xs[0]}))), "0");
    EXPECT_EQ(str(simplify(eq({xs[0], xs[0]}))), "1");
}


TEST_F(SimplifyTest, NotImpliesTruthTable)
{
    EXPECT_EQ(str(simplify(~impl(_zero, _zero))), "0");
    EXPECT_EQ(str(simplify(~impl(_zero, _one))), "0");
    EXPECT_EQ(str(simplify(~impl(_one, _zero))), "1");
    EXPECT_EQ(str(simplify(~impl(_one, _one))), "0");

    EXPECT_EQ(str(simplify(~impl(_zero, xs[0]))), "0");
    EXPECT_EQ(str(simplify(~impl(xs[0], _zero))), "x_0");
    EXPECT_EQ(str(simplify(~impl(_one, xs[0]))), "~x_0");
    EXPECT_EQ(str(simplify(~impl(xs[0], _one))), "0");
}


TEST_F(SimplifyTest, ImpliesTruthTable)
{
    EXPECT_EQ(str(simplify(impl(_zero, _zero))), "1");
    EXPECT_EQ(str(simplify(impl(_zero, _one))), "1");
    EXPECT_EQ(str(simplify(impl(_one, _zero))), "0");
    EXPECT_EQ(str(simplify(impl(_one, _one))), "1");

    EXPECT_EQ(str(simplify(impl(_zero, xs[0]))), "1");
    EXPECT_EQ(str(simplify(impl(xs[0], _zero))), "~x_0");
    EXPECT_EQ(str(simplify(impl(_one, xs[0]))), "x_0");
    EXPECT_EQ(str(simplify(impl(xs[0], _one))), "1");

    EXPECT_EQ(str(simplify(impl(xs[0], xs[0]))), "1");
    EXPECT_EQ(str(simplify(impl(~xs[0], xs[0]))), "x_0");
}


TEST_F(SimplifyTest, NotIfThenElseTruthTable)
{
    EXPECT_EQ(str(simplify(~ite(_zero, _zero, _zero))), "1");
    EXPECT_EQ(str(simplify(~ite(_zero, _zero, _one))), "0");
    EXPECT_EQ(str(simplify(~ite(_zero, _one, _zero))), "1");
    EXPECT_EQ(str(simplify(~ite(_zero, _one, _one))), "0");
    EXPECT_EQ(str(simplify(~ite(_one, _zero, _zero))), "1");
    EXPECT_EQ(str(simplify(~ite(_one, _zero, _one))), "1");
    EXPECT_EQ(str(simplify(~ite(_one, _one, _zero))), "0");
    EXPECT_EQ(str(simplify(~ite(_one, _one, _one))), "0");

    EXPECT_EQ(str(simplify(~ite(xs[0], _zero, _zero))), "1");
    EXPECT_EQ(str(simplify(~ite(xs[0], _zero, _one))), "x_0");
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], _zero, xs[1])), ~(~xs[0] & xs[1])));
    EXPECT_EQ(str(simplify(~ite(xs[0], _one, _zero))), "~x_0");
    EXPECT_EQ(str(simplify(~ite(xs[0], _one, _one))), "0");
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], _one, xs[1])), ~(xs[0] | xs[1])));
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], xs[1], _zero)), ~(xs[0] & xs[1])));
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], xs[1], _one)), ~(~xs[0] | xs[1])));
    EXPECT_EQ(str(simplify(~ite(xs[0], xs[1], xs[1]))), "~x_1");
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], xs[0], xs[1])), ~(xs[0] | xs[1])));
    EXPECT_TRUE(equivalent(simplify(~ite(xs[0], xs[1], xs[0])), ~(xs[0] & xs[1])));
}


TEST_F(SimplifyTest, IfThenElseTruthTable)
{
    EXPECT_EQ(str(simplify(ite(_zero, _zero, _zero))), "0");
    EXPECT_EQ(str(simplify(ite(_zero, _zero, _one))), "1");
    EXPECT_EQ(str(simplify(ite(_zero, _one, _zero))), "0");
    EXPECT_EQ(str(simplify(ite(_zero, _one, _one))), "1");
    EXPECT_EQ(str(simplify(ite(_one, _zero, _zero))), "0");
    EXPECT_EQ(str(simplify(ite(_one, _zero, _one))), "0");
    EXPECT_EQ(str(simplify(ite(_one, _one, _zero))), "1");
    EXPECT_EQ(str(simplify(ite(_one, _one, _one))), "1");

    EXPECT_EQ(str(simplify(ite(xs[0], _zero, _zero))), "0");
    EXPECT_EQ(str(simplify(ite(xs[0], _zero, _one))), "~x_0");
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], _zero, xs[1])), ~xs[0] & xs[1]));
    EXPECT_EQ(str(simplify(ite(xs[0], _one, _zero))), "x_0");
    EXPECT_EQ(str(simplify(ite(xs[0], _one, _one))), "1");
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], _one, xs[1])), xs[0] | xs[1]));
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], xs[1], _zero)), xs[0] & xs[1]));
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], xs[1], _one)), ~xs[0] | xs[1]));
    EXPECT_EQ(str(simplify(ite(xs[0], xs[1], xs[1]))), "x_1");
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], xs[0], xs[1])), xs[0] | xs[1]));
    EXPECT_TRUE(equivalent(simplify(ite(xs[0], xs[1], xs[0])), xs[0] & xs[1]));
}
