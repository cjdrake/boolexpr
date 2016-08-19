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


class TseytinTest : public BoolExprTest {};


TEST_F(TseytinTest, Atoms)
{
    auto ctx = Context();

    EXPECT_EQ(_zero->tseytin(ctx), _zero);
    EXPECT_EQ(_one->tseytin(ctx), _one);
    EXPECT_EQ(_log->tseytin(ctx), _log);
    EXPECT_EQ(_ill->tseytin(ctx), _ill);

    EXPECT_EQ((~xs[0])->tseytin(ctx), ~xs[0]);
    EXPECT_EQ(xs[0]->tseytin(ctx), xs[0]);
}


TEST_F(TseytinTest, Operators)
{
    auto ctx = Context();

    auto y0 =  nor_s({xs[0],  xor_s({xs[1], xs[2]}), xs[3]});
    auto y1 =   or_s({xs[0], xnor_s({xs[1], xs[2]}), xs[3]});
    auto y2 = nand_s({xs[0],   or_s({xs[1], xs[2]}), xs[3]});
    auto y3 =  and_s({xs[0],  nor_s({xs[1], xs[2]}), xs[3]});
    auto y4 = xnor_s({xs[0],  and_s({xs[1], xs[2]}), xs[3]});
    auto y5 =  xor_s({xs[0], nand_s({xs[1], xs[2]}), xs[3]});
    auto y6 =  neq_s({xs[0],   eq_s({xs[1], xs[2]}), xs[3]});
    auto y7 =   eq_s({xs[0],  neq_s({xs[1], xs[2]}), xs[3]});

    auto y8 = nimpl_s(xs[0],  or_s({xs[1], xs[2]}));
    auto y9 =  impl_s(xs[0], nor_s({xs[1], xs[2]}));

    auto y10 = nite_s(xs[0], xs[1],  and_s({xs[2], xs[3]}) );
    auto y11 =  ite_s(xs[0], xs[1], nand_s({xs[2], xs[3]}));

    EXPECT_TRUE(y0->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y1->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y2->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y3->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y4->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y5->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y6->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y7->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y8->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y9->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y10->tseytin(ctx)->is_cnf());
    EXPECT_TRUE(y11->tseytin(ctx)->is_cnf());
}


TEST_F(TseytinTest, CNF)
{
    auto ctx = Context();

    auto y0 = onehot({xs[0], xs[1], xs[2], xs[3]});
    auto y1 = y0->tseytin(ctx);

    EXPECT_TRUE(y0->is_cnf());
    EXPECT_EQ(y0->size(), y1->size());
}
