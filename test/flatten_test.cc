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


class FlattenTest : public BoolExprTest {};


TEST_F(FlattenTest, Atoms)
{
    EXPECT_EQ(_zero->to_cnf(), _zero);
    EXPECT_EQ(_one->to_cnf(), _one);
    EXPECT_EQ(_log->to_cnf(), _log);
    EXPECT_EQ(_ill->to_cnf(), _ill);
    EXPECT_EQ((~xs[0])->to_cnf(), ~xs[0]);
    EXPECT_EQ(xs[0]->to_cnf(), xs[0]);

    EXPECT_EQ(_zero->to_dnf(), _zero);
    EXPECT_EQ(_one->to_dnf(), _one);
    EXPECT_EQ(_log->to_dnf(), _log);
    EXPECT_EQ(_ill->to_dnf(), _ill);
    EXPECT_EQ((~xs[0])->to_dnf(), ~xs[0]);
    EXPECT_EQ(xs[0]->to_dnf(), xs[0]);
}


TEST_F(FlattenTest, Xor)
{
    for (size_t i = 0; i < 8; ++i) {
        vector<bx_t> args;
        for (size_t j = 0; j < i; ++j)
            args.push_back(xs[j]);

        auto y0 = xor_s(args);
        auto y1 = xnor_s(args);

        auto y0_cnf = y0->to_cnf();
        auto y0_dnf = y0->to_dnf();

        auto y1_cnf = y1->to_cnf();
        auto y1_dnf = y1->to_dnf();

        EXPECT_TRUE(y0->equiv(y0_cnf));
        EXPECT_TRUE(y0->equiv(y0_dnf));

        EXPECT_TRUE(y1->equiv(y1_cnf));
        EXPECT_TRUE(y1->equiv(y1_dnf));

        if (i == 0) {
            EXPECT_EQ(y0_cnf, _zero);
            EXPECT_EQ(y0_dnf, _zero);
            EXPECT_EQ(y1_cnf, _one);
            EXPECT_EQ(y1_dnf, _one);
        }

        if (i == 1) {
            EXPECT_EQ(y0_cnf, xs[0]);
            EXPECT_EQ(y0_dnf, xs[0]);
            EXPECT_EQ(y1_cnf, ~xs[0]);
            EXPECT_EQ(y1_dnf, ~xs[0]);
        }

        if (i >= 2) {
            EXPECT_TRUE(y0_cnf->is_cnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y0_cnf)->args.size(), (1<<(i-1)));

            EXPECT_TRUE(y0_dnf->is_dnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y0_dnf)->args.size(), (1<<(i-1)));

            EXPECT_TRUE(y1_cnf->is_cnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y1_cnf)->args.size(), (1<<(i-1)));

            EXPECT_TRUE(y1_dnf->is_dnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y1_dnf)->args.size(), (1<<(i-1)));
        }
    }
}


TEST_F(FlattenTest, Equal)
{
    for (size_t i = 0; i < 8; ++i) {
        vector<bx_t> args;
        for (size_t j = 0; j < i; ++j)
            args.push_back(xs[j]);

        auto y0 = eq_s(args);
        auto y1 = neq_s(args);

        auto y0_cnf = y0->to_cnf();
        auto y0_dnf = y0->to_dnf();

        auto y1_cnf = y1->to_cnf();
        auto y1_dnf = y1->to_dnf();

        EXPECT_TRUE(y0->equiv(y0_cnf));
        EXPECT_TRUE(y0->equiv(y0_dnf));

        EXPECT_TRUE(y1->equiv(y1_cnf));
        EXPECT_TRUE(y1->equiv(y1_dnf));

        if (i <= 1) {
            EXPECT_EQ(y0_cnf, _one);
            EXPECT_EQ(y0_dnf, _one);
            EXPECT_EQ(y1_cnf, _zero);
            EXPECT_EQ(y1_dnf, _zero);
        }

        if (i >= 2) {
            EXPECT_TRUE(y0_cnf->is_cnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y0_cnf)->args.size(), i*(i-1));

            EXPECT_TRUE(y0_dnf->is_dnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y0_dnf)->args.size(), 2);

            EXPECT_TRUE(y1_cnf->is_cnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y1_cnf)->args.size(), 2);

            EXPECT_TRUE(y1_dnf->is_dnf());
            EXPECT_EQ(std::static_pointer_cast<Operator const>(y1_dnf)->args.size(), i*(i-1));
        }
    }
}


TEST_F(FlattenTest, Implies)
{
    auto y0 = impl(xs[0], xs[1]);
    auto y1 = nimpl(xs[0], xs[1]);

    auto y0_cnf = y0->to_cnf();
    auto y0_dnf = y0->to_dnf();

    auto y1_cnf = y1->to_cnf();
    auto y1_dnf = y1->to_dnf();

    EXPECT_TRUE(y0->equiv(y0_cnf));
    EXPECT_TRUE(y0->equiv(y0_dnf));

    EXPECT_TRUE(y0_cnf->is_cnf());
    EXPECT_TRUE(y0_dnf->is_dnf());

    EXPECT_TRUE(y1->equiv(y1_cnf));
    EXPECT_TRUE(y1->equiv(y1_dnf));

    EXPECT_TRUE(y1_cnf->is_cnf());
    EXPECT_TRUE(y1_dnf->is_dnf());
}


TEST_F(FlattenTest, IfThenElse)
{
    auto y0 = ite(xs[0], xs[1], xs[2]);
    auto y1 = nite(xs[0], xs[1], xs[2]);

    auto y0_cnf = y0->to_cnf();
    auto y0_dnf = y0->to_dnf();

    auto y1_cnf = y1->to_cnf();
    auto y1_dnf = y1->to_dnf();

    EXPECT_TRUE(y0->equiv(y0_cnf));
    EXPECT_TRUE(y0->equiv(y0_dnf));

    EXPECT_TRUE(y0_cnf->is_cnf());
    EXPECT_TRUE(y0_dnf->is_dnf());

    EXPECT_TRUE(y1->equiv(y1_cnf));
    EXPECT_TRUE(y1->equiv(y1_dnf));

    EXPECT_TRUE(y1_cnf->is_cnf());
    EXPECT_TRUE(y1_dnf->is_dnf());
}


TEST_F(FlattenTest, All)
{
    auto y0 = or_({
                 nor({
                     xs[2], xs[6] | xs[0], xs[0] & xs[3], xs[5] ^ xs[1],
                 }),
                 and_({
                     xs[4], xs[1] | xs[0], xs[3] & xs[0], xs[1] ^ xs[2],
                 }),
                 xor_({
                     xs[7], xs[4] | xs[2], xs[6] & xs[1], xs[3] ^ xs[1],
                 }),
                 eq({
                     xs[0], xs[2] | xs[0], xs[7] & xs[2], xs[1] ^ xs[6],
                 }),
                 impl(xs[2], xs[3]),
                 ite(xs[3], xs[7], xs[0]),
             });

    auto y1 = and_({
                 or_({
                     xs[3], xs[0] | xs[2], xs[1] & xs[0], xs[3] ^ xs[2],
                 }),
                 nor({xs[3], xs[5]}),
                 nand({
                     xs[1], xs[2] | xs[3], xs[0] & xs[2], xs[1] ^ xs[3],
                 }),
                 xnor({
                     xs[3], xs[1] | xs[0], xs[0] & xs[2], xs[3] ^ xs[0],
                 }),
                 neq({
                     xs[0], xs[3] | xs[0], xs[0] & xs[1], xs[2] ^ xs[3],
                 }),
                 nimpl(xs[0], xs[2]),
                 nite(xs[1], xs[3], xs[2]),
              });

    auto y0_cnf = y0->to_cnf();
    auto y0_dnf = y0->to_dnf();

    EXPECT_TRUE(y0_cnf->is_cnf() && y0_cnf->equiv(y0));
    EXPECT_TRUE(y0_dnf->is_dnf() && y0_dnf->equiv(y0));

    auto y1_cnf = y1->to_cnf();
    auto y1_dnf = y1->to_dnf();

    EXPECT_TRUE(y1_cnf->is_cnf() && y1_cnf->equiv(y1));
    EXPECT_TRUE(y1_dnf->is_dnf() && y1_dnf->equiv(y1));
}
