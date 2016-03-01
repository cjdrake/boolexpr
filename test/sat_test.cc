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
    auto soln0 = _zero->sat();
    EXPECT_FALSE(soln0.first);

    auto soln1 = _one->sat();
    EXPECT_TRUE(soln1.first);
    auto p1 = *soln1.second;
    EXPECT_EQ(p1.size(), 0);

    auto soln2 = xs[0]->sat();
    EXPECT_TRUE(soln2.first);
    auto p2 = *soln2.second;
    EXPECT_EQ(p2.size(), 1);
    EXPECT_EQ(p2[xs[0]], _one);

    auto soln3 = (~xs[0])->sat();
    EXPECT_TRUE(soln3.first);
    auto p3 = *soln3.second;
    EXPECT_EQ(p3.size(), 1);
    EXPECT_EQ(p3[xs[0]], _zero);
}


TEST_F(SATTest, Clauses)
{
    auto y0 = ~xs[0] | xs[1] | ~xs[2] | xs[3];
    auto soln0 = y0->sat();
    EXPECT_TRUE(soln0.first);
    auto p0 = *soln0.second;
    EXPECT_EQ(p0.size(), 4);
    EXPECT_TRUE((p0[xs[0]] == _zero) || (p0[xs[1]] == _one) || (p0[xs[2]] == _zero) || (p0[xs[3]] == _one));

    auto y1 = ~xs[0] & xs[1] & ~xs[2] & xs[3];
    auto soln1 = y1->sat();
    EXPECT_TRUE(soln1.first);
    auto p1 = *soln1.second;
    EXPECT_EQ(p1.size(), 4);
    EXPECT_TRUE((p1[xs[0]] == _zero) && (p1[xs[1]] == _one) && (p1[xs[2]] == _zero) && (p1[xs[3]] == _one));
}


TEST_F(SATTest, Contradiction)
{
    auto y = (~xs[0] | ~xs[1]) & (~xs[0] | xs[1]) & (xs[0] | ~xs[1]) & (xs[0] | xs[1]);
    auto soln = y->sat();
    EXPECT_FALSE(soln.first);
}
