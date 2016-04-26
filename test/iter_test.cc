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


class IterTest : public BoolExprTest {};


TEST_F(IterTest, DFSIter)
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
    for (auto it = dfs_iter(y); it != dfs_iter(); ++it)
        EXPECT_EQ((*it)->to_string(), ans[i++]);
}
