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


class ArrayTest : public BoolExprTest {};


TEST_F(ArrayTest, Compose)
{
    auto bxa = Array({~xs[0], xs[1], ~xs[2], xs[3]});

    auto var2bx = var2bx_t {
        {xs[0], xs[4] | xs[5]},
        {xs[1], xs[6] & xs[7]},
        {xs[2], xs[8] ^ xs[9]},
        {xs[3], impl(xs[10], xs[11])},
    };

    auto bxa1 = bxa.compose(var2bx);
    EXPECT_TRUE(bxa1.items[0]->equiv(nor({xs[4], xs[5]})));
    EXPECT_TRUE(bxa1.items[1]->equiv(xs[6] & xs[7]));
    EXPECT_TRUE(bxa1.items[2]->equiv(xnor({xs[8], xs[9]})));
    EXPECT_TRUE(bxa1.items[3]->equiv(impl(xs[10], xs[11])));
}


TEST_F(ArrayTest, Restrict)
{
    auto bxa = Array({xs[0] | xs[1], xs[1] & xs[2], xs[2] ^ xs[3], impl(xs[3], xs[0])});

    auto p = point_t {
        {xs[0], _zero},
        {xs[1], _one},
        {xs[2], _zero},
        {xs[3], _one},
    };

    auto bxa1 = bxa.restrict_(p);
    EXPECT_EQ(bxa1.items[0], _one);
    EXPECT_EQ(bxa1.items[1], _zero);
    EXPECT_EQ(bxa1.items[2], _one);
    EXPECT_EQ(bxa1.items[3], _zero);
}


TEST_F(ArrayTest, Reduce)
{
    auto bxa = Array({~xs[0], xs[1], ~xs[2], xs[3]});

    auto y0 = bxa.or_reduce();
    EXPECT_TRUE(y0->equiv(~xs[0] | xs[1] | ~xs[2] | xs[3]));

    auto y1 = bxa.and_reduce();
    EXPECT_TRUE(y1->equiv(~xs[0] & xs[1] & ~xs[2] & xs[3]));

    auto y2 = bxa.xor_reduce();
    EXPECT_TRUE(y2->equiv(~xs[0] ^ xs[1] ^ ~xs[2] ^ xs[3]));
}
