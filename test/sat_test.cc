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


class SATTest : public BoolExprTest {};


TEST_F(SATTest, Atoms)
{
    // Zero is not satisfiable
    auto soln0 = _zero->sat();
    EXPECT_FALSE(soln0.first);

    // One is trivially satisfiable
    auto soln1 = _one->sat();
    EXPECT_TRUE(soln1.first);
    auto p1 = *soln1.second;
    EXPECT_EQ(p1.size(), 0);

    // Logical is not satisfiable
    auto soln2 = _log->sat();
    EXPECT_FALSE(soln2.first);

    // Illogical is not satisfiable
    auto soln3 = _ill->sat();
    EXPECT_FALSE(soln3.first);

    // sat(x)
    auto soln4 = xs[0]->sat();
    EXPECT_TRUE(soln4.first);
    auto p4 = *soln4.second;
    EXPECT_EQ(p4.size(), 1);
    EXPECT_EQ(p4[xs[0]], _one);

    // sat(~x)
    auto soln5 = (~xs[0])->sat();
    EXPECT_TRUE(soln5.first);
    auto p5 = *soln5.second;
    EXPECT_EQ(p5.size(), 1);
    EXPECT_EQ(p5[xs[0]], _zero);
}


TEST_F(SATTest, Clauses)
{
    // sat(~x0 | x1 | ~x2 | x3)
    auto y0 = or_s({~xs[0], xs[1], ~xs[2], xs[3]});
    auto soln0 = y0->sat();
    EXPECT_TRUE(soln0.first);
    auto p0 = *soln0.second;
    EXPECT_EQ(p0.size(), 4);
    EXPECT_TRUE((p0[xs[0]] == _zero) || (p0[xs[1]] == _one) || (p0[xs[2]] == _zero) || (p0[xs[3]] == _one));

    // sat(~x0 & x1 & ~x2 & x3)
    auto y1 = and_s({~xs[0], xs[1], ~xs[2], xs[3]});
    auto soln1 = y1->sat();
    EXPECT_TRUE(soln1.first);
    auto p1 = *soln1.second;
    EXPECT_EQ(p1.size(), 4);
    EXPECT_TRUE((p1[xs[0]] == _zero) && (p1[xs[1]] == _one) && (p1[xs[2]] == _zero) && (p1[xs[3]] == _one));
}


TEST_F(SATTest, Contradiction)
{
    auto y = and_s({~xs[0] | ~xs[1], ~xs[0] | xs[1], xs[0] | ~xs[1], xs[0] | xs[1]});
    auto soln = y->sat();
    EXPECT_FALSE(soln.first);
}


TEST_F(SATTest, Iter)
{
    int count;

    auto y0 = xs[0] | xs[1];

    count = 0;
    for (auto it = sat_iter(y0); it != sat_iter(); ++it, ++count);
    EXPECT_EQ(count, 3);

    auto y1 = xs[0] & xs[1];

    count = 0;
    for (auto it = sat_iter(y1); it != sat_iter(); ++it, ++count);
    EXPECT_EQ(count, 1);

    auto y2 = xs[0] ^ xs[1];

    count = 0;
    for (auto it = sat_iter(y2); it != sat_iter(); ++it, ++count);
    EXPECT_EQ(count, 2);

    auto y3 = zero();
    count = 0;
    for (auto it = sat_iter(y3); it != sat_iter(); ++it, ++count);
    EXPECT_EQ(count, 0);

    auto y4 = one();
    count = 0;
    for (auto it = sat_iter(y4); it != sat_iter(); ++it, ++count);
    EXPECT_EQ(count, 1);

    auto y5 = xs[0];
    count = 0;
    auto it5 = sat_iter(y5);
    auto soln5_it = (*it5).begin();
    EXPECT_EQ((*soln5_it).first, xs[0]);
    EXPECT_EQ((*soln5_it).second, _one);
    ++it5;
    EXPECT_EQ(it5, sat_iter());

    auto y6 = ~xs[0];
    count = 0;
    auto it6 = sat_iter(y6);
    auto soln6_it = (*it6).begin();
    EXPECT_EQ((*soln6_it).first, xs[0]);
    EXPECT_EQ((*soln6_it).second, _zero);
    ++it6;
    EXPECT_EQ(it6, sat_iter());
}
