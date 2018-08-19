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

class ToStringTest : public BoolExprTest {};

TEST_F(ToStringTest, Atoms) {
    EXPECT_EQ(_zero->to_string(), "0");
    EXPECT_EQ(_one->to_string(), "1");
    EXPECT_EQ(_log->to_string(), "X");
    EXPECT_EQ(_ill->to_string(), "?");

    EXPECT_EQ((~xs[0])->to_string(), "~x_0");
    EXPECT_EQ(xs[0]->to_string(), "x_0");
    EXPECT_EQ((~xs[1])->to_string(), "~x_1");
    EXPECT_EQ(xs[1]->to_string(), "x_1");
}

TEST_F(ToStringTest, Operators) {
    auto y0 = (~xs[0] & xs[1]) | (~xs[2] ^ xs[3]) | eq({~xs[4], xs[5]}) |
              impl(~xs[6], xs[7]) | ite(~xs[8], xs[9], ~xs[10]);

    EXPECT_EQ(y0->to_string(),
              "Or(Or(Or(Or("
              "And(~x_0, x_1), "
              "Xor(~x_2, x_3)), "
              "Equal(~x_4, x_5)), "
              "Implies(~x_6, x_7)), "
              "IfThenElse(~x_8, x_9, ~x_10))");

    auto y1 = ~(~(~xs[0] & xs[1]) | ~(~xs[2] ^ xs[3]) | ~eq({~xs[4], xs[5]}) |
                ~impl(~xs[6], xs[7]) | ~ite(~xs[8], xs[9], ~xs[10]));

    EXPECT_EQ(y1->to_string(),
              "Nor(Or(Or(Or("
              "Nand(~x_0, x_1), "
              "Xnor(~x_2, x_3)), "
              "Unequal(~x_4, x_5)), "
              "NotImplies(~x_6, x_7)), "
              "NotIfThenElse(~x_8, x_9, ~x_10))");

    auto y2 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);

    EXPECT_EQ(y2->to_string(), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))");
}
