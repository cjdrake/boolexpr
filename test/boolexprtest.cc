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

BoolExprTest::BoolExprTest()
    : _zero{zero()},
      _one{one()},
      _log{logical()},
      _ill{illogical()},
      p{ctx.get_var("p")},
      q{ctx.get_var("q")},
      s{ctx.get_var("s")},
      d1{ctx.get_var("d1")},
      d0{ctx.get_var("d0")} {
    for (int i = 0; i < N; ++i)
        xs.push_back(ctx.get_var("x_" + std::to_string(i)));
}

void BoolExprTest::SetUp() {}

void BoolExprTest::TearDown() {}
