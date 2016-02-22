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


class BinOpTest : public BoolExprTest {};


TEST_F(BinOpTest, Or)
{
    auto y1 = or_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(str(to_binop(y1)), "Or(x_0, Or(x_1, x_2))");

    auto y2 = or_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(str(to_binop(y2)), "Or(Or(x_0, x_1), Or(x_2, x_3))");
}


TEST_F(BinOpTest, And)
{
    auto y1 = and_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(str(to_binop(y1)), "And(x_0, And(x_1, x_2))");

    auto y2 = and_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(str(to_binop(y2)), "And(And(x_0, x_1), And(x_2, x_3))");
}


TEST_F(BinOpTest, Xor)
{
    auto y1 = xor_({xs[0], xs[1], xs[2]});
    EXPECT_EQ(str(to_binop(y1)), "Xor(x_0, Xor(x_1, x_2))");

    auto y2 = xor_({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(str(to_binop(y2)), "Xor(Xor(x_0, x_1), Xor(x_2, x_3))");
}


TEST_F(BinOpTest, Equal)
{
    auto y1 = eq({xs[0], xs[1]});
    EXPECT_EQ(str(to_binop(y1)), "Equal(x_0, x_1)");

    auto y2 = eq({xs[0], xs[1], xs[2]});
    EXPECT_EQ(str(to_binop(y2)), "And(Equal(x_0, x_1), Equal(x_0, x_2), Equal(x_1, x_2))");

    auto y3 = eq({xs[0], xs[1], xs[2], xs[3]});
    EXPECT_EQ(str(to_binop(y3)), "And(Equal(x_0, x_1), Equal(x_0, x_2), Equal(x_0, x_3), Equal(x_1, x_2), Equal(x_1, x_3), Equal(x_2, x_3))");
}
