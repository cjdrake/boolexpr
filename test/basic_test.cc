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


TEST_F(BoolExprTest, Basic)
{
    EXPECT_EQ(ctx.get_var("x_0"), xs[0]);

    EXPECT_EQ(~_log, _log);
    EXPECT_EQ(~_ill, _ill);

    auto x = std::static_pointer_cast<const Literal>(xs[0]);
    auto xn = std::static_pointer_cast<const Literal>(~xs[0]);
    auto y = std::static_pointer_cast<const Literal>(xs[1]);

    EXPECT_LT(x, y);
    EXPECT_LT(xn, x);

    vector<lit_t> lits;
    lits.push_back(std::static_pointer_cast<const Literal>(xs[7]));
    lits.push_back(std::static_pointer_cast<const Literal>(xs[13]));
    lits.push_back(std::static_pointer_cast<const Literal>(~xs[3]));
    lits.push_back(std::static_pointer_cast<const Literal>(xs[5]));
    lits.push_back(std::static_pointer_cast<const Literal>(~xs[13]));
    lits.push_back(std::static_pointer_cast<const Literal>(xs[3]));
    lits.push_back(std::static_pointer_cast<const Literal>(~xs[5]));
    std::sort(lits.begin(), lits.end());
    EXPECT_EQ(lits[0]->id, 3<<1);      // ~xs[3]
    EXPECT_EQ(lits[1]->id, 3<<1 | 1);  //  xs[3]
    EXPECT_EQ(lits[5]->id, 13<<1);     // ~xs[13]
    EXPECT_EQ(lits[6]->id, 13<<1 | 1); //  xs[13]

    EXPECT_TRUE(_zero->equiv(_zero));
    EXPECT_TRUE(_one->equiv(_one));
    EXPECT_TRUE((~x)->equiv(~x));
    EXPECT_TRUE(x->equiv(x));
}


TEST_F(BoolExprTest, Constructors)
{
    auto args = vector<bx_t> {xs[0], xs[1], xs[2], xs[3]};

    auto y0 = nor(args);
    auto y1 = nor(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y2 = nor({xs[0], xs[1], xs[2], xs[3]});
    auto y3 = nor_s(args);
    auto y4 = nor_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y5 = nor_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y0, y1, y2, y3, y4, y5})->sat().first);

    auto  y6 = or_(args);
    auto  y7 = or_(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto  y8 = or_({xs[0], xs[1], xs[2], xs[3]});
    auto  y9 = or_s(args);
    auto y10 = or_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y11 = or_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y6, y7, y8, y9, y10, y11})->sat().first);

    auto y12 = nand(args);
    auto y13 = nand(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y14 = nand({xs[0], xs[1], xs[2], xs[3]});
    auto y15 = nand_s(args);
    auto y16 = nand_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y17 = nand_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y12, y13, y14, y15, y16, y17})->sat().first);

    auto y18 = and_(args);
    auto y19 = and_(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y20 = and_({xs[0], xs[1], xs[2], xs[3]});
    auto y21 = and_s(args);
    auto y22 = and_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y23 = and_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y18, y19, y20, y21, y22, y23})->sat().first);

    auto y24 = xnor(args);
    auto y25 = xnor(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y26 = xnor({xs[0], xs[1], xs[2], xs[3]});
    auto y27 = xnor_s(args);
    auto y28 = xnor_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y29 = xnor_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y24, y25, y26, y27, y28, y29})->sat().first);

    auto y30 = xor_(args);
    auto y31 = xor_(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y32 = xor_({xs[0], xs[1], xs[2], xs[3]});
    auto y33 = xor_s(args);
    auto y34 = xor_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y35 = xor_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y30, y31, y32, y33, y34, y35})->sat().first);

    auto y36 = neq(args);
    auto y37 = neq(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y38 = neq({xs[0], xs[1], xs[2], xs[3]});
    auto y39 = neq_s(args);
    auto y40 = neq_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y41 = neq_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y36, y37, y38, y39, y40, y41})->sat().first);

    auto y42 = eq(args);
    auto y43 = eq(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y44 = eq({xs[0], xs[1], xs[2], xs[3]});
    auto y45 = eq_s(args);
    auto y46 = eq_s(vector<bx_t>{xs[0], xs[1], xs[2], xs[3]});
    auto y47 = eq_s({xs[0], xs[1], xs[2], xs[3]});

    EXPECT_TRUE(eq({y42, y43, y44, y45, y46, y47})->sat().first);
}


TEST_F(BoolExprTest, ToString)
{
    EXPECT_EQ(_zero->to_string(), "0");
    EXPECT_EQ(_one->to_string(), "1");
    EXPECT_EQ(_log->to_string(), "X");
    EXPECT_EQ(_ill->to_string(), "?");

    EXPECT_EQ(xs[0], xs[0]);
    EXPECT_EQ(~xs[0], ~xs[0]);

    EXPECT_EQ((~xs[0])->to_string(), "~x_0");
    EXPECT_EQ(xs[0]->to_string(), "x_0");
    EXPECT_EQ((~xs[1])->to_string(), "~x_1");
    EXPECT_EQ(xs[1]->to_string(), "x_1");

    auto y0 = (~xs[0] & xs[1])
            | (~xs[2] ^ xs[3])
            | eq({~xs[4], xs[5]})
            | impl(~xs[6], xs[7])
            | ite(~xs[8], xs[9], ~xs[10]);

    EXPECT_EQ(y0->to_string(), "Or(Or(Or(Or(And(~x_0, x_1), Xor(~x_2, x_3)), Equal(~x_4, x_5)), Implies(~x_6, x_7)), IfThenElse(~x_8, x_9, ~x_10))");

    auto y1 = ~(~(~xs[0] & xs[1])
            | ~(~xs[2] ^ xs[3])
            | ~eq({~xs[4], xs[5]})
            | ~impl(~xs[6], xs[7])
            | ~ite(~xs[8], xs[9], ~xs[10]));

    EXPECT_EQ(y1->to_string(), "Nor(Or(Or(Or(Nand(~x_0, x_1), Xnor(~x_2, x_3)), Unequal(~x_4, x_5)), NotImplies(~x_6, x_7)), NotIfThenElse(~x_8, x_9, ~x_10))");

    auto y2 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);

    EXPECT_EQ(y2->to_string(), "Or(~x_0, Xor(And(x_1, ~x_2), x_3))");
}


TEST_F(BoolExprTest, Degenerate)
{
    auto zero_args = vector<bx_t> {};
    auto one_arg = vector<bx_t> {xs[0]};

    EXPECT_EQ(nor(zero_args), _one);
    EXPECT_EQ(nor(one_arg), ~xs[0]);
    EXPECT_EQ(nor(vector<bx_t> {}), _one);
    EXPECT_EQ(nor(vector<bx_t> {xs[0]}), ~xs[0]);
    EXPECT_EQ(nor({}), _one);
    EXPECT_EQ(nor({xs[0]}), ~xs[0]);

    EXPECT_EQ(or_(zero_args), _zero);
    EXPECT_EQ(or_(one_arg), xs[0]);
    EXPECT_EQ(or_(vector<bx_t> {}), _zero);
    EXPECT_EQ(or_(vector<bx_t> {xs[0]}), xs[0]);
    EXPECT_EQ(or_({}), _zero);
    EXPECT_EQ(or_({xs[0]}), xs[0]);

    EXPECT_EQ(nand(zero_args), _zero);
    EXPECT_EQ(nand(one_arg), ~xs[0]);
    EXPECT_EQ(nand(vector<bx_t> {}), _zero);
    EXPECT_EQ(nand(vector<bx_t> {xs[0]}), ~xs[0]);
    EXPECT_EQ(nand({}), _zero);
    EXPECT_EQ(nand({xs[0]}), ~xs[0]);

    EXPECT_EQ(and_(zero_args), _one);
    EXPECT_EQ(and_(one_arg), xs[0]);
    EXPECT_EQ(and_(vector<bx_t> {}), _one);
    EXPECT_EQ(and_(vector<bx_t> {xs[0]}), xs[0]);
    EXPECT_EQ(and_({}), _one);
    EXPECT_EQ(and_({xs[0]}), xs[0]);

    EXPECT_EQ(xnor(zero_args), _one);
    EXPECT_EQ(xnor(one_arg), ~xs[0]);
    EXPECT_EQ(xnor(vector<bx_t> {}), _one);
    EXPECT_EQ(xnor(vector<bx_t> {xs[0]}), ~xs[0]);
    EXPECT_EQ(xnor({}), _one);
    EXPECT_EQ(xnor({xs[0]}), ~xs[0]);

    EXPECT_EQ(xor_(zero_args), _zero);
    EXPECT_EQ(xor_(one_arg), xs[0]);
    EXPECT_EQ(xor_(vector<bx_t> {}), _zero);
    EXPECT_EQ(xor_(vector<bx_t> {xs[0]}), xs[0]);
    EXPECT_EQ(xor_({}), _zero);
    EXPECT_EQ(xor_({xs[0]}), xs[0]);

    EXPECT_EQ(neq(zero_args), _zero);
    EXPECT_EQ(neq(one_arg), _zero);
    EXPECT_EQ(neq(vector<bx_t> {}), _zero);
    EXPECT_EQ(neq(vector<bx_t> {xs[0]}), _zero);
    EXPECT_EQ(neq({}), _zero);
    EXPECT_EQ(neq({xs[0]}), _zero);

    EXPECT_EQ(eq(zero_args), _one);
    EXPECT_EQ(eq(one_arg), _one);
    EXPECT_EQ(eq(vector<bx_t> {}), _one);
    EXPECT_EQ(eq(vector<bx_t> {xs[0]}), _one);
    EXPECT_EQ(eq({}), _one);
    EXPECT_EQ(eq({xs[0]}), _one);
}


TEST_F(BoolExprTest, IsCNF)
{
    EXPECT_FALSE(_zero->is_cnf());
    EXPECT_TRUE(_one->is_cnf());
    EXPECT_FALSE((xs[0] ^ xs[1])->is_cnf());

    EXPECT_TRUE(~xs[0]->is_cnf());
    EXPECT_TRUE(xs[0]->is_cnf());

    auto y0 = xs[0] & xs[1];
    EXPECT_TRUE(y0->is_cnf());

    auto y1 = xs[0] & _one;
    EXPECT_FALSE(y1->is_cnf());

    auto y2 = xs[0] & (xs[1] | _zero);
    EXPECT_FALSE(y2->is_cnf());

    auto y3 = xs[0] | xs[1];
    EXPECT_TRUE(y3->is_cnf());

    auto y4 = (xs[0] & xs[1]) | (xs[2] & xs[3]);
    EXPECT_FALSE(y4->is_cnf());

    auto y5 = (xs[0] | xs[1]) & (xs[2] | xs[3]);
    EXPECT_TRUE(y5->is_cnf());
}


TEST_F(BoolExprTest, IsDNF)
{
    EXPECT_TRUE(_zero->is_dnf());
    EXPECT_FALSE(_one->is_dnf());
    EXPECT_FALSE((xs[0] ^ xs[1])->is_dnf());

    EXPECT_TRUE(~xs[0]->is_dnf());
    EXPECT_TRUE(xs[0]->is_dnf());

    auto y0 = xs[0] & xs[1];
    EXPECT_TRUE(y0->is_dnf());

    auto y1 = xs[0] | _zero;
    EXPECT_FALSE(y1->is_dnf());

    auto y2 = xs[0] | (xs[1] & _one);
    EXPECT_FALSE(y2->is_dnf());

    auto y3 = xs[0] & xs[1];
    EXPECT_TRUE(y3->is_dnf());

    auto y4 = (xs[0] | xs[1]) & (xs[2] | xs[3]);
    EXPECT_FALSE(y4->is_dnf());

    auto y5 = (xs[0] & xs[1]) | (xs[2] & xs[3]);
    EXPECT_TRUE(y5->is_dnf());
}


TEST_F(BoolExprTest, Support)
{
    auto y = (~xs[0] & xs[1]) | (~xs[2] & xs[3]);
    std::unordered_set<var_t> s = {xs[0], xs[1], xs[2], xs[3]};

    EXPECT_EQ(y->support(), s);
}
