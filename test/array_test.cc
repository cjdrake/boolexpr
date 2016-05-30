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


class ArrayTest : public BoolExprTest {};


TEST_F(ArrayTest, Compose)
{
    Array X {~xs[0], xs[1], ~xs[2], xs[3]};

    auto var2bx = var2bx_t {
        {xs[0], xs[4] | xs[5]},
        {xs[1], xs[6] & xs[7]},
        {xs[2], xs[8] ^ xs[9]},
        {xs[3], impl(xs[10], xs[11])},
    };

    auto Y = X.compose(var2bx);
    EXPECT_TRUE(Y->items[0]->equiv(nor({xs[4], xs[5]})));
    EXPECT_TRUE(Y->items[1]->equiv(xs[6] & xs[7]));
    EXPECT_TRUE(Y->items[2]->equiv(xnor({xs[8], xs[9]})));
    EXPECT_TRUE(Y->items[3]->equiv(impl(xs[10], xs[11])));
}


TEST_F(ArrayTest, Restrict)
{
    Array X {
        xs[0] | xs[1],
        xs[1] & xs[2],
        xs[2] ^ xs[3],
        impl(xs[3], xs[0])
    };

    auto point = point_t {
        {xs[0], _zero},
        {xs[1], _one},
        {xs[2], _zero},
        {xs[3], _one},
    };

    auto Y = X.restrict_(point);
    EXPECT_EQ(Y->items[0], _one);
    EXPECT_EQ(Y->items[1], _zero);
    EXPECT_EQ(Y->items[2], _one);
    EXPECT_EQ(Y->items[3], _zero);
}


TEST_F(ArrayTest, Reduce)
{
    Array X {~xs[0], xs[1], ~xs[2], xs[3]};

    auto Y0 = X.or_reduce();
    EXPECT_TRUE(Y0->equiv(~xs[0] | xs[1] | ~xs[2] | xs[3]));

    auto Y1 = X.and_reduce();
    EXPECT_TRUE(Y1->equiv(~xs[0] & xs[1] & ~xs[2] & xs[3]));

    auto Y2 = X.xor_reduce();
    EXPECT_TRUE(Y2->equiv(~xs[0] ^ xs[1] ^ ~xs[2] ^ xs[3]));
}


TEST_F(ArrayTest, Equiv)
{
    Array X0 {
        xs[0], xs[1] ^ xs[2],
        eq({xs[3], xs[4]}),
        impl(xs[5], xs[6]),
        ite(xs[7], xs[8], xs[9])
    };
    Array X1 {
        xs[0],
        (~xs[1] & xs[2]) | (xs[1] & ~xs[2]),
        (~xs[3] & ~xs[4]) | (xs[3] & xs[4]),
        ~xs[5] | xs[6],
        (xs[7] & xs[8]) | (~xs[7] & xs[9])
    };
    Array X2 {xs[0], xs[1]};
    Array X3 {xs[0], xs[1], xs[2], xs[3], xs[4]};

    EXPECT_TRUE(X0.equiv(X1));
    EXPECT_FALSE(X0.equiv(X2));
    EXPECT_FALSE(X0.equiv(X3));
}


TEST_F(ArrayTest, LeftShift)
{
    Array A {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], xs[6], xs[7]};
    Array si {xs[10], xs[11], xs[12]};

    Array B {xs[10], xs[11], xs[12], xs[0], xs[1], xs[2], xs[3], xs[4]};
    Array so {xs[5], xs[6], xs[7]};

    auto pair = A.lsh(si);
    EXPECT_TRUE(pair.first->equiv(B));
    EXPECT_TRUE(pair.second->equiv(so));

}


TEST_F(ArrayTest, RightShift)
{
    Array A {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], xs[6], xs[7]};
    Array si {xs[10], xs[11], xs[12]};

    Array B {xs[3], xs[4], xs[5], xs[6], xs[7], xs[10], xs[11], xs[12]};
    Array so {xs[0], xs[1], xs[2]};

    auto pair = A.rsh(si);
    EXPECT_TRUE(pair.first->equiv(so));
    EXPECT_TRUE(pair.second->equiv(B));
}


TEST_F(ArrayTest, ArithmeticRightShift)
{
    Array A {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], xs[6], xs[7]};

    Array B {xs[3], xs[4], xs[5], xs[6], xs[7], xs[7], xs[7], xs[7]};
    Array so {xs[0], xs[1], xs[2]};

    auto pair = A.arsh(3);
    EXPECT_TRUE(pair.first->equiv(so));
    EXPECT_TRUE(pair.second->equiv(B));
}
