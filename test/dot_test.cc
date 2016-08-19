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

#include <sstream>


class ToDotTest : public BoolExprTest {};


TEST_F(ToDotTest, Atoms)
{
    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; n" << &*_zero << " [label=\"0\",shape=box]; }";
    EXPECT_EQ(_zero->to_dot(), ss0.str());

    std::ostringstream ss1;
    ss1 << "graph { rankdir=BT; n" << &*_one << " [label=\"1\",shape=box]; }";
    EXPECT_EQ(_one->to_dot(), ss1.str());

    std::ostringstream ss2;
    ss2 << "graph { rankdir=BT; n" << &*_log << " [label=\"X\",shape=box]; }";
    EXPECT_EQ(_log->to_dot(), ss2.str());

    std::ostringstream ss3;
    ss3 << "graph { rankdir=BT; n" << &*_ill << " [label=\"?\",shape=box]; }";
    EXPECT_EQ(_ill->to_dot(), ss3.str());

    std::ostringstream ss4;
    ss4 << "graph { rankdir=BT; n" << &*(~xs[0]) << " [label=\"~x_0\",shape=box]; }";
    EXPECT_EQ((~xs[0])->to_dot(), ss4.str());

    std::ostringstream ss5;
    ss5 << "graph { rankdir=BT; n" << &*(xs[0]) << " [label=\"x_0\",shape=box]; }";
    EXPECT_EQ(xs[0]->to_dot(), ss5.str());
}


TEST_F(ToDotTest, Nor)
{
    auto y0 = ~(xs[0] | xs[1]);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~or\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Or)
{
    auto y0 = xs[0] | xs[1];

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"or\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Nand)
{
    auto y0 = ~(xs[0] & xs[1]);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~and\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, And)
{
    auto y0 = xs[0] & xs[1];

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"and\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Xnor)
{
    auto y0 = ~(xs[0] ^ xs[1]);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~xor\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Xor)
{
    auto y0 = xs[0] ^ xs[1];

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"xor\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Unequal)
{
    auto y0 = ~eq({xs[0], xs[1]});

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~eq\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Equal)
{
    auto y0 = eq({xs[0], xs[1]});

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*xs[0] << " [label=\"x_0\",shape=box]; ";
    ss0 << "n" << &*xs[1] << " [label=\"x_1\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"eq\",shape=circle]; ";
    ss0 << "n" << &*xs[0] << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*xs[1] << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, NotImplies)
{
    auto y0 = ~impl(p, q);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*p << " [label=\"p\",shape=box]; ";
    ss0 << "n" << &*q << " [label=\"q\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~impl\",shape=circle]; ";
    ss0 << "n" << &*p << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*q << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, Implies)
{
    auto y0 = impl(p, q);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*p << " [label=\"p\",shape=box]; ";
    ss0 << "n" << &*q << " [label=\"q\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"impl\",shape=circle]; ";
    ss0 << "n" << &*p << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*q << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, NotIfThenElse)
{
    auto y0 = ~ite(s, d1, d0);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*s << " [label=\"s\",shape=box]; ";
    ss0 << "n" << &*d1 << " [label=\"d1\",shape=box]; ";
    ss0 << "n" << &*d0 << " [label=\"d0\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"~ite\",shape=circle]; ";
    ss0 << "n" << &*s << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*d1 << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*d0 << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}


TEST_F(ToDotTest, IfThenElse)
{
    auto y0 = ite(s, d1, d0);

    std::ostringstream ss0;
    ss0 << "graph { rankdir=BT; ";
    ss0 << "n" << &*s << " [label=\"s\",shape=box]; ";
    ss0 << "n" << &*d1 << " [label=\"d1\",shape=box]; ";
    ss0 << "n" << &*d0 << " [label=\"d0\",shape=box]; ";
    ss0 << "n" << &*y0 << " [label=\"ite\",shape=circle]; ";
    ss0 << "n" << &*s << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*d1 << " -- " << "n" << &*y0 << "; ";
    ss0 << "n" << &*d0 << " -- " << "n" << &*y0 << "; ";
    ss0 << "}";

    EXPECT_EQ(y0->to_dot(), ss0.str());
}
