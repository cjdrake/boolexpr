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


class NNFTest : public BoolExprTest {};


TEST_F(NNFTest, DeMorgan)
{
    auto y0 = ~(xs[0] | xs[1]);
    auto y1 = y0->to_nnf();
    EXPECT_TRUE(IS_AND(y1) && y0->equiv(y1));

    auto y2 = ~(xs[0] & xs[1]);
    auto y3 = y2->to_nnf();
    EXPECT_TRUE(IS_OR(y3) && y2->equiv(y3));
}


TEST_F(NNFTest, Xor)
{
    auto y0 = ~(xs[0] ^ xs[1]);
    auto y1 = y0->to_nnf();
    EXPECT_TRUE(IS_AND(y1) && y0->equiv(y1));

    auto y2 = xs[0] ^ xs[1];
    auto y3 = y2->to_nnf();
    EXPECT_TRUE(IS_OR(y3) && y2->equiv(y3));

    auto y4 = xor_({xs[0], xs[1], xs[2]});
    auto y5 = y4->to_nnf();
    EXPECT_TRUE(IS_OR(y5) && y4->equiv(y5));

    auto y6 = xor_({xs[0], xs[1], xs[2], xs[3]});
    auto y7 = y6->to_nnf();
    EXPECT_TRUE(IS_OR(y7) && y6->equiv(y7));
}


TEST_F(NNFTest, Equal)
{
    auto y0 = ~eq({xs[0], xs[1]});
    auto y1 = y0->to_nnf();
    EXPECT_TRUE(IS_AND(y1) && y0->equiv(y1));

    auto y2 = eq({xs[0], xs[1]});
    auto y3 = y2->to_nnf();
    EXPECT_TRUE(IS_OR(y3) && y2->equiv(y3));

    auto y4 = eq({xs[0], xs[1], xs[2]});
    auto y5 = y4->to_nnf();
    EXPECT_TRUE(IS_OR(y5) && y4->equiv(y5));

    auto y6 = eq({xs[0], xs[1], xs[2], xs[3]});
    auto y7 = y6->to_nnf();
    EXPECT_TRUE(IS_OR(y7) && y6->equiv(y7));
}


TEST_F(NNFTest, Implies)
{
    auto y0 = ~impl(xs[0], xs[1]);
    auto y1 = y0->to_nnf();
    EXPECT_TRUE(IS_AND(y1) && y0->equiv(y1));

    auto y2 = impl(xs[0], xs[1]);
    auto y3 = y2->to_nnf();
    EXPECT_TRUE(IS_OR(y3) && y2->equiv(y3));
}


TEST_F(NNFTest, IfThenElse)
{
    auto y0 = ~ite(xs[0], xs[1], xs[2]);
    auto y1 = y0->to_nnf();
    EXPECT_TRUE(IS_AND(y1) && y0->equiv(y1));

    auto y2 = ite(xs[0], xs[1], xs[2]);
    auto y3 = y2->to_nnf();
    EXPECT_TRUE(IS_OR(y3) && y2->equiv(y3));
}
