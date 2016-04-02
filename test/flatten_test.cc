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
    auto y0 = xor_s({xs[0], xs[1], xs[2], xs[3], xs[4], xs[5]});

    auto y1 = y0->to_cnf();
    auto y2 = std::static_pointer_cast<const Operator>(y1);
    EXPECT_TRUE(IS_AND(y2) && y2->args.size() == 32 && y0->equiv(y2));

    auto y3 = y0->to_dnf();
    auto y4 = std::static_pointer_cast<const Operator>(y3);
    EXPECT_TRUE(IS_OR(y4) && y4->args.size() == 32 && y0->equiv(y4));
}
