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


class SATTest : public BoolExprTest {};


TEST_F(SATTest, Atoms)
{
    auto soln1 = sat(_zero);
    EXPECT_FALSE(soln1.first);

    auto soln2 = sat(_one);
    EXPECT_TRUE(soln2.first);
    auto p2 = *soln2.second;
    EXPECT_EQ(p2.size(), 0);

    auto soln3 = sat(xs[0]);
    EXPECT_TRUE(soln3.first);
    auto p3 = *soln3.second;
    EXPECT_EQ(p3.size(), 1);
    EXPECT_EQ(p3[xs[0]], _one);

    auto soln4 = sat(~xs[0]);
    EXPECT_TRUE(soln4.first);
    auto p4 = *soln4.second;
    EXPECT_EQ(p4.size(), 1);
    EXPECT_EQ(p4[xs[0]], _zero);
}


TEST_F(SATTest, Clauses)
{
    auto soln1 = sat(~xs[0] | xs[1] | ~xs[2] | xs[3]);
    EXPECT_TRUE(soln1.first);
    auto p1 = *soln1.second;
    EXPECT_EQ(p1.size(), 4);
    EXPECT_TRUE((p1[xs[0]] == _zero) || (p1[xs[1]] == _one) || (p1[xs[2]] == _zero) || (p1[xs[3]] == _one));

    auto soln2 = sat(~xs[0] & xs[1] & ~xs[2] & xs[3]);
    EXPECT_TRUE(soln2.first);
    auto p2 = *soln2.second;
    EXPECT_EQ(p2.size(), 4);
    EXPECT_TRUE((p2[xs[0]] == _zero) && (p2[xs[1]] == _one) && (p2[xs[2]] == _zero) && (p2[xs[3]] == _one));
}


TEST_F(SATTest, Contradiction)
{
    auto f = (~xs[0] | ~xs[1]) & (~xs[0] | xs[1]) & (xs[0] | ~xs[1]) & (xs[0] | xs[1]);
    auto soln = sat(f);
    EXPECT_FALSE(soln.first);
}
