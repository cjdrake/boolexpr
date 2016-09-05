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


class IterTest : public BoolExprTest {};


static bool
parity(unsigned int n)
{
    bool parity = false;
    while (n) {
        parity = !parity;
        n = n & (n - 1);
    }
    return parity;
}


TEST_F(IterTest, DFSIter)
{
    // x_1 is listed twice in the leaves,
    // but should only be yielded once by the DFS iterator.
    auto y = (~xs[0] & xs[1]) | (~xs[2] & xs[1]);

    const std::string ans[] = {
        "~x_0",
        "x_1",
        "And(~x_0, x_1)",
        "~x_2",
        "And(~x_2, x_1)",
        "Or(And(~x_0, x_1), And(~x_2, x_1))",
    };

    size_t i = 0;
    for (auto it = dfs_iter(y); it != dfs_iter(); ++it)
        EXPECT_EQ((*it)->to_string(), ans[i++]);
}


TEST_F(IterTest, SpaceIter)
{
    vector<vector<bool>> ans {
        {false, false, false},
        {true,  false, false},
        {false, true,  false},
        {true,  true,  false},
        {false, false, true},
        {true,  false, true},
        {false, true,  true},
        {true,  true,  true},
    };

    size_t i = 0;
    for (auto it = space_iter(3); it != space_iter(); ++it) {
        for (size_t j = 0; j < 3; ++j)
            EXPECT_EQ((*it)[j], ans[i][j]);
        EXPECT_EQ(it.parity(), parity(i));
        i += 1;
    }
}


TEST_F(IterTest, PointsIter)
{
    vector<point_t> ans {
        { {xs[0], _zero}, {xs[1], _zero} },
        { {xs[0],  _one}, {xs[1], _zero} },
        { {xs[0], _zero}, {xs[1],  _one} },
        { {xs[0],  _one}, {xs[1],  _one} },
    };

    vector<var_t> vars {xs[0], xs[1]};

    size_t i = 0;
    for (auto it = points_iter(vars); it != points_iter(); ++it)
        EXPECT_EQ(*it, ans[i++]);
}


TEST_F(IterTest, TermsIter)
{
    vector<vector<bx_t>> ans {
        { ~xs[0], ~xs[1] },
        {  xs[0], ~xs[1] },
        { ~xs[0],  xs[1] },
        {  xs[0],  xs[1] },
    };

    vector<bx_t> bxs {xs[0], xs[1]};

    size_t i = 0;
    for (auto it = terms_iter(bxs); it != terms_iter(); ++it)
        EXPECT_EQ(*it, ans[i++]);
}


TEST_F(IterTest, DomainIter)
{
    vector<point_t> ans {
        { {xs[0], _zero}, {xs[1], _zero} },
        { {xs[0],  _one}, {xs[1], _zero} },
        { {xs[0], _zero}, {xs[1],  _one} },
        { {xs[0],  _one}, {xs[1],  _one} },
    };

    // Domain iteration variable ordering cannot be predicted.
    vector<bool> seen {false, false, false, false};

    auto f = xs[0] | xs[1];

    for (auto it = domain_iter(f); it != domain_iter(); ++it) {
        for (size_t j = 0; j < ans.size(); ++j)
            if (*it == ans[j]) {
                seen[j] = true;
                break;
            }
    }

    for (auto const & item : seen)
        EXPECT_TRUE(item);
}


TEST_F(IterTest, CofactorIter)
{
    vector<bx_t> ans {_zero, xs[2], xs[2], _one};

    auto f = (xs[0] & xs[1]) | (xs[0] & xs[2]) | (xs[1] & xs[2]);
    auto vars = vector<var_t> {xs[0], xs[1]};

    size_t i = 0;
    for (auto it = cf_iter(f, vars); it != cf_iter(); ++it)
        EXPECT_TRUE((*it)->equiv(ans[i++]));
}
