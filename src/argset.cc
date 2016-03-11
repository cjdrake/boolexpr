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

#include "boolexpr/boolexpr.h"
#include "argset.h"


using namespace boolexpr;


LatticeArgSet::LatticeArgSet(vector<bx_t> const & args, BoolExpr::Kind const & kind,
                             bx_t const & identity, bx_t const & dominator)
    : infimum {true}
    , supremum {false}
    , kind {kind}
    , identity {identity}
    , dominator {dominator}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
LatticeArgSet::insert(bx_t const & arg)
{
    // 1 + x <=> 1 ; x + 0 <=> x
    if (supremum || ARE_SAME(arg, identity))
        return;

    // FIXME(cjdrake): Implement unknowns

    // x + 1 <=> 1 ; x + ~x <=> 1
    if (ARE_SAME(arg, dominator) || (IS_LIT(arg) && (args.find(~arg) != args.end()))) {
        infimum = false;
        supremum = true;
        args.clear();
        return;
    }

    // x + (y + z) <=> x + y + z
    if (arg->kind == kind) {
        auto op = std::static_pointer_cast<Operator const>(arg);
        for (bx_t const & _arg : op->args) insert(_arg);
        return;
    }

    // x + x <=> x
    infimum = false;
    args.insert(arg);
}


bx_t
LatticeArgSet::reduce() const
{
    if (infimum) return identity;
    if (supremum) return dominator;

    if (args.size() == 0) return identity;
    if (args.size() == 1) return *args.begin();

    return to_op();
}


OrArgSet::OrArgSet(vector<bx_t> const & args)
    : LatticeArgSet(args, BoolExpr::OR, Or::identity(), Or::dominator())
{}


bx_t
OrArgSet::to_op() const
{
    return std::make_shared<Or>(true, vector<bx_t>(args.begin(), args.end()));
}


AndArgSet::AndArgSet(vector<bx_t> const & args)
    : LatticeArgSet(args, BoolExpr::AND, And::identity(), And::dominator())
{}


bx_t
AndArgSet::to_op() const
{
    return std::make_shared<And>(true, vector<bx_t>(args.begin(), args.end()));
}


XorArgSet::XorArgSet(vector<bx_t> const & args)
    : parity {true}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
XorArgSet::insert(bx_t const & arg)
{
    if (IS_KNOWN(arg)) {
        parity ^= static_cast<bool>(arg->kind);
        return;
    }

    // FIXME(cjdrake): Implement unknowns

    // xor(x, y, z, z) <=> xor(x, y) ; xnor(x, y, z, z) <=> xnor(x, y)
    auto search = args.find(arg);
    if (search != args.end()) {
        args.erase(search);
        return;
    }

    // xor(x, y, z, ~z) <=> xnor(x, y) ; xnor(x, y, z, ~z) <=> xor(x, y)
    if (IS_LIT(arg)) {
        auto search = args.find(~arg);
        if (search != args.end()) {
            args.erase(search);
            parity ^= true;
            return;
        }
    }

    // xor(x, xor(y, z)) <=> xor(x, y, z) ; xnor(x, xor(y, z)) <=> xnor(x, y, z)
    if (IS_XOR(arg)) {
        auto op = std::static_pointer_cast<Operator const>(arg);
        for (bx_t const & _arg : op->args) insert(_arg);
        return;
    }

    // xor(x, xnor(y, z)) <=> xnor(x, y, z) ; xnor(x, xnor(y, z)) <=> xor(x, y, z)
    if (IS_XNOR(arg)) {
        auto op = std::static_pointer_cast<Operator const>(arg);
        for (bx_t const & _arg : op->args) insert(_arg);
        parity ^= true;
        return;
    }

    args.insert(arg);
}


bx_t
XorArgSet::to_op() const
{
    return std::make_shared<Xor>(true, vector<bx_t>(args.begin(), args.end()));
}


bx_t
XorArgSet::reduce() const
{
    bx_t ret;

    if (args.size() == 0)
        ret = zero();
    else if (args.size() == 1)
        ret = *args.begin();
    else
        ret = to_op();

    return parity ? ret : ~ret;
}


EqArgSet::EqArgSet(vector<bx_t> const & args)
    : has_zero {false}
    , has_one {false}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
EqArgSet::insert(bx_t const & arg)
{
    if (has_zero && has_one)
        return;

    // eq(0, x) <=> 0
    if (IS_ZERO(arg)) {
        has_zero = true;
        if (has_one) args.clear();
        return;
    }

    // eq(1, x) <=> 1
    if (IS_ONE(arg)) {
        has_one = true;
        if (has_zero) args.clear();
        return;
    }

    // eq(~x, x) <=> 0
    if (IS_LIT(arg) && (args.find(~arg) != args.end())) {
        has_zero = true;
        has_one = true;
        args.clear();
        return;
    }

    // eq(x, x, y) <=> eq(x, y)
    args.insert(arg);
}


bx_t
EqArgSet::to_op() const
{
    return std::make_shared<Equal>(true, vector<bx_t>(args.begin(), args.end()));
}


bx_t
EqArgSet::reduce() const
{
    // eq(0, 1) <=> 0
    if (has_zero && has_one)
        return zero();

    // eq() <=> eq(0) <=> eq(1) <=> 1
    if ((static_cast<size_t>(has_zero) + static_cast<size_t>(has_one) + args.size()) < 2)
        return one();

    // eq(0, x, y) <=> nor(x, y)
    if (has_zero)
        return nor_s(vector<bx_t>(args.begin(), args.end()));

    // eq(1, x, y) <=> x & y
    if (has_one)
        return and_s(vector<bx_t>(args.begin(), args.end()));

    return to_op();
}
