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


class ComposeTest : public BoolExprTest {};


TEST_F(ComposeTest, Basic)
{
    auto var2bx = var2bx_t {
        {xs[0], xs[4]},
        {xs[1], xs[5]},
        {xs[2], xs[6]},
        {xs[3], xs[7]},
    };

    auto g0 = _zero->compose(var2bx);
    EXPECT_EQ(g0, _zero);

    auto g1 = _one->compose(var2bx);
    EXPECT_EQ(g1, _one);

    auto f2 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);
    auto g2 = f2->compose(var2bx);
    EXPECT_TRUE(g2->equiv(~xs[4] | ((xs[5] & ~xs[6]) ^ xs[7])));
}


TEST_F(ComposeTest, Restrict)
{
    auto point = point_t {
        {xs[0], _zero},
        {xs[1], _one},
        {xs[2], _zero},
        {xs[3], _one},
    };

    auto g0 = _zero->restrict_(point);
    EXPECT_EQ(g0, _zero);

    auto g1 = _one->restrict_(point);
    EXPECT_EQ(g1, _one);

    auto f2 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);
    auto g2 = f2->restrict_(point);
    EXPECT_EQ(g2, _one);
}
