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


#ifndef BOOLEXPRTEST_H_
#define BOOLEXPRTEST_H_


using namespace boolexpr;
using std::vector;


class BoolExprTest : public ::testing::Test
{
protected:
    const int N = 1024;

    zero_t _zero;
    one_t _one;
    log_t _log;
    ill_t _ill;

    Context ctx;

    vector<var_t> xs;

    var_t p, q;
    var_t s, d1, d0;

    virtual void SetUp();
    virtual void TearDown();

public:
    BoolExprTest();
};


#endif  // BOOLEXPRTEST_H_
