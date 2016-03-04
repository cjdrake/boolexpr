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


class CountTest : public BoolExprTest {};


TEST_F(CountTest, Atoms)
{
    EXPECT_EQ(_zero->depth(), 0);
    EXPECT_EQ(_one->depth(), 0);
    EXPECT_EQ(_log->depth(), 0);
    EXPECT_EQ(_ill->depth(), 0);
    EXPECT_EQ((~xs[0])->depth(), 0);
    EXPECT_EQ(xs[0]->depth(), 0);

    EXPECT_EQ(_zero->size(), 1);
    EXPECT_EQ(_one->size(), 1);
    EXPECT_EQ(_log->size(), 1);
    EXPECT_EQ(_ill->size(), 1);
    EXPECT_EQ((~xs[0])->size(), 1);
    EXPECT_EQ(xs[0]->size(), 1);

    EXPECT_EQ(_zero->atom_count(), 1);
    EXPECT_EQ(_one->atom_count(), 1);
    EXPECT_EQ(_log->atom_count(), 1);
    EXPECT_EQ(_ill->atom_count(), 1);
    EXPECT_EQ((~xs[0])->atom_count(), 1);
    EXPECT_EQ(xs[0]->atom_count(), 1);

    EXPECT_EQ(_zero->op_count(), 0);
    EXPECT_EQ(_one->op_count(), 0);
    EXPECT_EQ(_log->op_count(), 0);
    EXPECT_EQ(_ill->op_count(), 0);
    EXPECT_EQ((~xs[0])->op_count(), 0);
    EXPECT_EQ(xs[0]->op_count(), 0);
}


TEST_F(CountTest, Operators)
{
    auto y0 = or_({
                  (~xs[0] & xs[1]),
                  (~xs[2] ^ xs[3]),
                  eq({~xs[4], xs[5]}),
                  impl(~xs[6], xs[7]),
                  ite(~xs[8], xs[9], ~xs[10]),
              });

    EXPECT_EQ(y0->depth(), 2);
    EXPECT_EQ(y0->size(), 17);
    EXPECT_EQ(y0->atom_count(), 11);
    EXPECT_EQ(y0->op_count(), 6);

    auto y1 = ~xs[0] | ((xs[1] & ~xs[2]) ^ xs[3]);

    EXPECT_EQ(y1->depth(), 3);
    EXPECT_EQ(y1->size(), 7);
    EXPECT_EQ(y1->atom_count(), 4);
    EXPECT_EQ(y1->op_count(), 3);

    auto y2 = xor_({xs[0], xs[1], xs[2], xs[3]})->to_nnf();

    // Or(
    //     And(Or(x2, ~x3), Or(~x2, x3), Or(And(x0, ~x1), And(~x0, x1))),
    //     And(Or(x0, ~x1), Or(~x0, x1), Or(And(x2, ~x3), And(~x2, x3))),
    // )

    EXPECT_EQ(y2->depth(), 4);
    EXPECT_EQ(y2->size(), 29);
    EXPECT_EQ(y2->atom_count(), 16);
    EXPECT_EQ(y2->op_count(), 13);
}
