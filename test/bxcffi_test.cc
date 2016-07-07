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


TEST(CFFI, Basic)
{
    auto zero = boolexpr_zero();
    auto one = boolexpr_one();
    auto log = boolexpr_logical();
    auto ill = boolexpr_illogical();

    auto cstr_0 = boolexpr_BoolExpr_to_string(zero);
    EXPECT_STREQ(cstr_0, "0");

    auto cstr_1 = boolexpr_BoolExpr_to_string(one);
    EXPECT_STREQ(cstr_1, "1");

    auto cstr_x = boolexpr_BoolExpr_to_string(log);
    EXPECT_STREQ(cstr_x, "X");

    auto cstr_xx = boolexpr_BoolExpr_to_string(ill);
    EXPECT_STREQ(cstr_xx, "?");

    auto ctx = boolexpr_Context_new();

    auto a = boolexpr_Context_get_var(ctx, "a");
    auto b = boolexpr_Context_get_var(ctx, "b");
    auto c = boolexpr_Context_get_var(ctx, "c");
    auto d = boolexpr_Context_get_var(ctx, "d");

    auto an = boolexpr_not(a);
    auto bn = boolexpr_not(b);
    auto cn = boolexpr_not(c);
    auto dn = boolexpr_not(d);

    auto cstr_a = boolexpr_BoolExpr_to_string(a);
    auto cstr_b = boolexpr_BoolExpr_to_string(b);
    auto cstr_c = boolexpr_BoolExpr_to_string(c);
    auto cstr_d = boolexpr_BoolExpr_to_string(d);
    EXPECT_STREQ(cstr_a, "a");
    EXPECT_STREQ(cstr_b, "b");
    EXPECT_STREQ(cstr_c, "c");
    EXPECT_STREQ(cstr_d, "d");

    auto cstr_an = boolexpr_BoolExpr_to_string(an);
    auto cstr_bn = boolexpr_BoolExpr_to_string(bn);
    auto cstr_cn = boolexpr_BoolExpr_to_string(cn);
    auto cstr_dn = boolexpr_BoolExpr_to_string(dn);
    EXPECT_STREQ(cstr_an, "~a");
    EXPECT_STREQ(cstr_bn, "~b");
    EXPECT_STREQ(cstr_cn, "~c");
    EXPECT_STREQ(cstr_dn, "~d");

    void const * args[] = {an, b, cn, d};

    auto y0 = boolexpr_or(4, args);
    auto y1 = boolexpr_and(4, args);
    auto y2 = boolexpr_xor(4, args);
    auto y3 = boolexpr_eq(4, args);
    auto y4 = boolexpr_impl(an, b);
    auto y5 = boolexpr_ite(an, b, cn);

    EXPECT_EQ(boolexpr_BoolExpr_kind(y0), BoolExpr::OR);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y1), BoolExpr::AND);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y2), BoolExpr::XOR);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y3), BoolExpr::EQ);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y4), BoolExpr::IMPL);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y5), BoolExpr::ITE);

    EXPECT_EQ(boolexpr_BoolExpr_depth(y0), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y1), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y2), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y3), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y4), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y5), 1);

    auto cstr_y0 = boolexpr_BoolExpr_to_string(y0);
    auto cstr_y1 = boolexpr_BoolExpr_to_string(y1);
    auto cstr_y2 = boolexpr_BoolExpr_to_string(y2);
    auto cstr_y3 = boolexpr_BoolExpr_to_string(y3);
    auto cstr_y4 = boolexpr_BoolExpr_to_string(y4);
    auto cstr_y5 = boolexpr_BoolExpr_to_string(y5);

    EXPECT_STREQ(cstr_y0, "Or(~a, b, ~c, d)");
    EXPECT_STREQ(cstr_y1, "And(~a, b, ~c, d)");
    EXPECT_STREQ(cstr_y2, "Xor(~a, b, ~c, d)");
    EXPECT_STREQ(cstr_y3, "Equal(~a, b, ~c, d)");
    EXPECT_STREQ(cstr_y4, "Implies(~a, b)");
    EXPECT_STREQ(cstr_y5, "IfThenElse(~a, b, ~c)");

    boolexpr_String_del(cstr_0);
    boolexpr_String_del(cstr_1);
    boolexpr_String_del(cstr_x);
    boolexpr_String_del(cstr_xx);
    boolexpr_String_del(cstr_a);
    boolexpr_String_del(cstr_b);
    boolexpr_String_del(cstr_c);
    boolexpr_String_del(cstr_d);
    boolexpr_String_del(cstr_an);
    boolexpr_String_del(cstr_bn);
    boolexpr_String_del(cstr_cn);
    boolexpr_String_del(cstr_dn);
    boolexpr_String_del(cstr_y0);
    boolexpr_String_del(cstr_y1);
    boolexpr_String_del(cstr_y2);
    boolexpr_String_del(cstr_y3);
    boolexpr_String_del(cstr_y4);
    boolexpr_String_del(cstr_y5);
}
