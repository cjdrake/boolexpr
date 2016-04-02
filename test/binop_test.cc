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
#include <cryptominisat4/cryptominisat.h>
#include <gtest/gtest.h>

#include "boolexpr/boolexpr.h"
#include "boolexprtest.h"


using namespace boolexpr;


class BinOpTest : public BoolExprTest {};


TEST_F(BinOpTest, Or)
{
    auto y0 = or_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(y0->to_binop()->to_string(), "Or(x_0, Or(x_1, x_2))");

    auto y1 = or_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y1->to_binop()->to_string(), "Or(Or(x_0, x_1), Or(x_2, x_3))");

    auto y2 = nor({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y2->to_binop()->to_string(), "Nor(Or(x_0, x_1), Or(x_2, x_3))");
}


TEST_F(BinOpTest, And)
{
    auto y0 = and_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(y0->to_binop()->to_string(), "And(x_0, And(x_1, x_2))");

    auto y1 = and_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y1->to_binop()->to_string(), "And(And(x_0, x_1), And(x_2, x_3))");

    auto y2 = nand({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y2->to_binop()->to_string(), "Nand(And(x_0, x_1), And(x_2, x_3))");
}


TEST_F(BinOpTest, Xor)
{
    auto y0 = xor_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(y0->to_binop()->to_string(), "Xor(x_0, Xor(x_1, x_2))");

    auto y1 = xor_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y1->to_binop()->to_string(), "Xor(Xor(x_0, x_1), Xor(x_2, x_3))");

    auto y2 = xnor({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y2->to_binop()->to_string(), "Xnor(Xor(x_0, x_1), Xor(x_2, x_3))");
}


TEST_F(BinOpTest, Equal)
{
    auto y0 = eq({xs[0], xs[1]});
    EXPECT_EQ(y0->to_binop()->to_string(), "Equal(x_0, x_1)");

    auto y1 = eq({xs[0], xs[1], xs[2]});
    EXPECT_EQ(y1->to_binop()->to_string(), "And(Equal(x_0, x_1), Equal(x_0, x_2), Equal(x_1, x_2))");

    auto y2 = eq({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y2->to_binop()->to_string(), "And(Equal(x_0, x_1), Equal(x_0, x_2), Equal(x_0, x_3), Equal(x_1, x_2), Equal(x_1, x_3), Equal(x_2, x_3))");

    auto y3 = neq({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(y3->to_binop()->to_string(), "Nand(Equal(x_0, x_1), Equal(x_0, x_2), Equal(x_0, x_3), Equal(x_1, x_2), Equal(x_1, x_3), Equal(x_2, x_3))");
}


TEST_F(BinOpTest, Implies)
{
    auto y0 = impl(or_({xs[0], xs[1], xs[2], xs[3]}),
                   and_({xs[0], xs[1], xs[2], xs[3]})
              );
    EXPECT_EQ(y0->to_binop()->to_string(),
              "Implies(Or(Or(x_0, x_1), Or(x_2, x_3)), And(And(x_0, x_1), And(x_2, x_3)))");

    auto y1 = nimpl(or_({xs[0], xs[1], xs[2], xs[3]}),
                    and_({xs[0], xs[1], xs[2], xs[3]})
              );
    EXPECT_EQ(y1->to_binop()->to_string(),
              "NotImplies(Or(Or(x_0, x_1), Or(x_2, x_3)), And(And(x_0, x_1), And(x_2, x_3)))");
}


TEST_F(BinOpTest, IfThenElse)
{
    auto y0 = ite(or_({xs[0], xs[1], xs[2], xs[3]}),
                  and_({xs[0], xs[1], xs[2], xs[3]}),
                  xor_({xs[0], xs[1], xs[2], xs[3]}));
    EXPECT_EQ(y0->to_binop()->to_string(),
              "IfThenElse(Or(Or(x_0, x_1), Or(x_2, x_3)), And(And(x_0, x_1), And(x_2, x_3)), Xor(Xor(x_0, x_1), Xor(x_2, x_3)))");

    auto y1 = nite(or_({xs[0], xs[1], xs[2], xs[3]}),
                   and_({xs[0], xs[1], xs[2], xs[3]}),
                   xor_({xs[0], xs[1], xs[2], xs[3]}));
    EXPECT_EQ(y1->to_binop()->to_string(),
              "NotIfThenElse(Or(Or(x_0, x_1), Or(x_2, x_3)), And(And(x_0, x_1), And(x_2, x_3)), Xor(Xor(x_0, x_1), Xor(x_2, x_3)))");
}
