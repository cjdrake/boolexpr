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

#include "boolexpr.h"
#include "boolexprtest.h"


using namespace boolexpr;


void
BoolExprTest::SetUp()
{
    _zero = zero();
    _one = one();
    _logical = logical();
    _illogical = illogical();

    // Initialize local literals
    for (int i = 0; i < N; ++i)
        xs.push_back(ctx.get_var("x_" + std::to_string(i)));
}


void
BoolExprTest::TearDown() {}
