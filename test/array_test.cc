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


class ArrayTest : public BoolExprTest {};


TEST_F(ArrayTest, Constructors)
{
    // empty constructor
    Array X0;

    // lval ref
    auto args = vector<bx_t> {xs[0], xs[1], xs[2]};
    auto X1 = Array(args);

    // rval ref
    auto X2 = Array {xs[0], xs[1], xs[2], xs[3]};

    EXPECT_EQ(X0.size(), 0);
    EXPECT_EQ(X1.size(), 3);
    EXPECT_EQ(X2.size(), 4);
}


TEST_F(ArrayTest, BitWiseOps)
{
    auto X0 = Array {xs[0], xs[1], xs[2], xs[3]};
    auto X1 = Array {xs[4], xs[5]};
    auto X2 = Array {xs[6], xs[7], xs[8], xs[9], xs[10], xs[11]};

    auto Y0 = ~X0;
    EXPECT_TRUE(Y0->equiv(Array {~xs[0], ~xs[1], ~xs[2], ~xs[3]}));

    auto Y1 = X0 | X1;
    auto Y2 = X0 | X2;
    EXPECT_TRUE(Y1->equiv(Array {xs[0] | xs[4], xs[1] | xs[5], xs[2], xs[3]}));
    EXPECT_TRUE(Y2->equiv(Array {xs[0] | xs[6], xs[1] | xs[7], xs[2] | xs[8], xs[3] | xs[9], xs[10], xs[11]}));

    auto Y3 = X0 & X1;
    auto Y4 = X0 & X2;
    EXPECT_TRUE(Y3->equiv(Array {xs[0] & xs[4], xs[1] & xs[5], xs[2], xs[3]}));
    EXPECT_TRUE(Y4->equiv(Array {xs[0] & xs[6], xs[1] & xs[7], xs[2] & xs[8], xs[3] & xs[9], xs[10], xs[11]}));

    auto Y5 = X0 ^ X1;
    auto Y6 = X0 ^ X2;
    EXPECT_TRUE(Y5->equiv(Array {xs[0] ^ xs[4], xs[1] ^ xs[5], xs[2], xs[3]}));
    EXPECT_TRUE(Y6->equiv(Array {xs[0] ^ xs[6], xs[1] ^ xs[7], xs[2] ^ xs[8], xs[3] ^ xs[9], xs[10], xs[11]}));

    delete Y0;
    delete Y1;
    delete Y2;
    delete Y3;
    delete Y4;
    delete Y5;
    delete Y6;
}


TEST_F(ArrayTest, ConcatOp)
{
    auto X0 = Array {xs[0], xs[1], xs[2], xs[3]};
    auto X1 = Array {xs[4], xs[5]};

    auto Y = X0 + X1;
    EXPECT_TRUE(Y->equiv(Array {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5]}));

    delete Y;
}


TEST_F(ArrayTest, RepeatOp)
{
    auto X0 = Array {xs[0], xs[1], xs[2]};

    auto Y0 = 3 * X0;
    auto Y1 = X0 * 3;

    EXPECT_TRUE(Y0->equiv(Array {xs[0], xs[1], xs[2], xs[0], xs[1], xs[2], xs[0], xs[1], xs[2]}));
    EXPECT_TRUE(Y1->equiv(Array {xs[0], xs[1], xs[2], xs[0], xs[1], xs[2], xs[0], xs[1], xs[2]}));

    delete Y0;
    delete Y1;
}


TEST_F(ArrayTest, Compose)
{
    Array X {~xs[0], xs[1], ~xs[2], xs[3], ~xs[4], xs[5]};

    auto var2bx = var2bx_t {
        { xs[0], xs[10] | xs[11] },
        { xs[1], xs[12] & xs[13] },
        { xs[2], xs[14] ^ xs[15] },
        { xs[3], eq({xs[16], xs[17]}) },
        { xs[4], impl(p, q) },
        { xs[5], ite(s, d1, d0) },
    };

    auto Y0 = X.compose(var2bx);

    // expected
    Array Y1 {
        nor({xs[10], xs[11]}),
        xs[12] & xs[13],
        xnor({xs[14], xs[15]}),
        eq({xs[16], xs[17]}),
        nimpl(p, q),
        ite(s, d1, d0),
    };

    EXPECT_TRUE(Y0->equiv(Y1));

    delete Y0;
}


TEST_F(ArrayTest, Restrict)
{
    Array X {
        xs[0] | xs[1],
        xs[1] & xs[2],
        xs[2] ^ xs[3],
        eq({xs[4], xs[5]}),
        impl(p, q),
        ite(s, d1, d0),
    };

    auto point = point_t {
        {xs[0], _zero},
        {xs[1], _one},
        {xs[2], _zero},
        {xs[3], _one},
        {xs[4], _zero},
        {xs[5], _one},
        {p, _zero},
        {q, _one},
        {s, _zero},
        {d1, _one},
        {d0, _zero},
    };

    auto Y0 = X.restrict_(point);

    // expected
    Array Y1 {_one, _zero, _one, _zero, _one, _zero};

    EXPECT_TRUE(Y0->equiv(Y1));

    delete Y0;
}


TEST_F(ArrayTest, Equivalent)
{
    Array X0 {xs[0] | xs[1], xs[2] & xs[3], xs[4] ^ xs[5]};
    Array X1 {xs[0] | xs[1], xs[2] & xs[3], xs[4] ^ xs[5]};
    Array X2 {xs[0] | xs[1], xs[2] & xs[3]};
    Array X3 {xs[0] | xs[1], xs[2] ^ xs[3], xs[4] & xs[5]};

    EXPECT_TRUE(X0.equiv(X1));
    EXPECT_FALSE(X0.equiv(X2));
    EXPECT_FALSE(X0.equiv(X3));
}


TEST_F(ArrayTest, ZeroExtend)
{
    Array X {~xs[0], xs[1], ~xs[2], xs[3]};

    auto Y0 = X.zext(2);

    Array Y1 {~xs[0], xs[1], ~xs[2], xs[3], _zero, _zero};

    EXPECT_TRUE(Y0->equiv(Y1));

    delete Y0;
}


TEST_F(ArrayTest, SignExtend)
{
    Array X {~xs[0], xs[1], ~xs[2], xs[3]};

    auto Y0 = X.sext(2);

    Array Y1 {~xs[0], xs[1], ~xs[2], xs[3], xs[3], xs[3]};

    EXPECT_TRUE(Y0->equiv(Y1));

    delete Y0;
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


TEST_F(ArrayTest, LeftShift)
{
    Array A {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], xs[6], xs[7]};
    Array si {xs[10], xs[11], xs[12]};

    Array B {xs[10], xs[11], xs[12], xs[0], xs[1], xs[2], xs[3], xs[4]};
    Array so {xs[5], xs[6], xs[7]};

    auto pair = A.lsh(si);
    EXPECT_TRUE(pair.first->equiv(B));
    EXPECT_TRUE(pair.second->equiv(so));

    delete pair.first;
    delete pair.second;
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

    delete pair.first;
    delete pair.second;
}


TEST_F(ArrayTest, ArithmeticRightShift)
{
    Array A {xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], xs[6], xs[7]};

    Array B {xs[3], xs[4], xs[5], xs[6], xs[7], xs[7], xs[7], xs[7]};
    Array so {xs[0], xs[1], xs[2]};

    auto pair = A.arsh(3);

    EXPECT_TRUE(pair.first->equiv(so));
    EXPECT_TRUE(pair.second->equiv(B));

    delete pair.first;
    delete pair.second;
}
