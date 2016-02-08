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


#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>
#include <gtest/gtest.h>

#include "boolexpr.h"
#include "boolexprtest.h"


using namespace boolexpr;


TEST_F(BoolExprTest, Basic)
{
    EXPECT_EQ(ctx.get_var("x_0"), xs[0]);

    auto x = std::static_pointer_cast<Literal>(xs[0]);
    auto xn = std::static_pointer_cast<Literal>(~xs[0]);
    auto y = std::static_pointer_cast<Literal>(xs[1]);

    EXPECT_LT(x, y);
    EXPECT_LT(xn, x);

    EXPECT_LT(~or_({xs[0], xs[1]}), or_({xs[0], xs[1]}));
    EXPECT_LT(~and_({xs[0], xs[1]}), and_({xs[0], xs[1]}));
    EXPECT_LT(~xor_({xs[0], xs[1]}), xor_({xs[0], xs[1]}));

    vector<lit_t> lits;
    lits.push_back(std::static_pointer_cast<Literal>(xs[7]));
    lits.push_back(std::static_pointer_cast<Literal>(xs[13]));
    lits.push_back(std::static_pointer_cast<Literal>(~xs[3]));
    lits.push_back(std::static_pointer_cast<Literal>(xs[5]));
    lits.push_back(std::static_pointer_cast<Literal>(~xs[13]));
    lits.push_back(std::static_pointer_cast<Literal>(xs[3]));
    lits.push_back(std::static_pointer_cast<Literal>(~xs[5]));
    std::sort(lits.begin(), lits.end());
    EXPECT_EQ(lits[0]->id, 3<<1);      // ~xs[3]
    EXPECT_EQ(lits[1]->id, 3<<1 | 1);  // xs[3]
    EXPECT_EQ(lits[5]->id, 13<<1);     // ~xs[13]
    EXPECT_EQ(lits[6]->id, 13<<1 | 1); // xs[13]
}


TEST_F(BoolExprTest, ToString)
{
    EXPECT_EQ(str(_zero), "0");
    EXPECT_EQ(str(_one), "1");
    EXPECT_EQ(str(_logical), "X");
    EXPECT_EQ(str(_illogical), "?");
    EXPECT_EQ(str(~_logical), "X");
    EXPECT_EQ(str(~_illogical), "?");

    EXPECT_EQ(xs[0], xs[0]);
    EXPECT_EQ(~xs[0], ~xs[0]);

    EXPECT_EQ(str(~xs[0]), "~x_0");
    EXPECT_EQ(str(xs[0]), "x_0");
    EXPECT_EQ(str(~xs[1]), "~x_1");
    EXPECT_EQ(str(xs[1]), "x_1");

    auto y0 = (~xs[0] & xs[1])
            | (~xs[2] ^ xs[3])
            | eq({~xs[4], xs[5]})
            | impl(~xs[6], xs[7])
            | ite(~xs[8], xs[9], ~xs[10]);

    EXPECT_EQ(str(y0), "Or(Or(Or(Or(And(~x_0, x_1), Xor(~x_2, x_3)), Equal(~x_4, x_5)), Implies(~x_6, x_7)), IfThenElse(~x_8, x_9, ~x_10))");

    auto y1 = ~(~(~xs[0] & xs[1])
            | ~(~xs[2] ^ xs[3])
            | ~eq({~xs[4], xs[5]})
            | ~impl(~xs[6], xs[7])
            | ~ite(~xs[8], xs[9], ~xs[10]));

    EXPECT_EQ(str(y1), "Nor(Or(Or(Or(Nand(~x_0, x_1), Xnor(~x_2, x_3)), Unequal(~x_4, x_5)), NotImplies(~x_6, x_7)), NotIfThenElse(~x_8, x_9, ~x_10))");

    auto y2 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);

    EXPECT_EQ(str(y2), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))");
}


TEST_F(BoolExprTest, Degenerate)
{
    EXPECT_EQ(or_({}), _zero);
    EXPECT_EQ(or_({xs[0]}), xs[0]);
    EXPECT_EQ(and_({}), _one);
    EXPECT_EQ(and_({xs[0]}), xs[0]);
    EXPECT_EQ(xor_({}), _zero);
    EXPECT_EQ(xor_({xs[0]}), xs[0]);
    EXPECT_EQ(eq({}), _one);
    EXPECT_EQ(eq({xs[0]}), _one);
}


TEST_F(BoolExprTest, IsDNF)
{
    EXPECT_TRUE(_zero->is_dnf());
    EXPECT_FALSE(_one->is_dnf());
    EXPECT_FALSE((~xs[0] ^ xs[1])->is_dnf());

    EXPECT_TRUE(~xs[0]->is_dnf());
    EXPECT_TRUE(xs[0]->is_dnf());

    auto y0 = ~xs[0] & xs[1];
    EXPECT_TRUE(y0->is_dnf());

    auto y1 = ~xs[0] | xs[1];
    EXPECT_TRUE(y1->is_dnf());

    auto y2 = (~xs[0] & xs[1]) | (~xs[2] & xs[3]);
    EXPECT_TRUE(y2->is_dnf());
}


TEST_F(BoolExprTest, IsCNF)
{
    EXPECT_FALSE(_zero->is_cnf());
    EXPECT_TRUE(_one->is_cnf());
    EXPECT_FALSE((~xs[0] ^ xs[1])->is_cnf());

    EXPECT_TRUE(~xs[0]->is_cnf());
    EXPECT_TRUE(xs[0]->is_cnf());

    auto y0 = ~xs[0] & xs[1];
    EXPECT_TRUE(y0->is_cnf());

    auto y1 = ~xs[0] | xs[1];
    EXPECT_TRUE(y1->is_cnf());

    auto y2 = (~xs[0] | xs[1]) & (~xs[2] | xs[3]);
    EXPECT_TRUE(y2->is_cnf());
}


TEST_F(BoolExprTest, DFSIter)
{
    auto y = (~xs[0] & xs[1]) | (~xs[2] & xs[3]);
    const string ans[] = {
        "~x_0",
        "x_1",
        "And(~x_0, x_1)",
        "~x_2",
        "x_3",
        "And(~x_2, x_3)",
        "Or(And(~x_0, x_1), And(~x_2, x_3))",
    };

    int i = 0;
    for (const auto& node : y)
        EXPECT_EQ(str(node), ans[i++]);
}


TEST_F(BoolExprTest, Support)
{
    auto y = (~xs[0] & xs[1]) | (~xs[2] & xs[3]);
    unordered_set<var_t> s = {xs[0], xs[1], xs[2], xs[3]};

    EXPECT_EQ(support(y), s);
}
