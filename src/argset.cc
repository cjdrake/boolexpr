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

#include "boolexpr/boolexpr.h"
#include "argset.h"


using namespace boolexpr;


LatticeArgSet::LatticeArgSet(vector<bx_t> const & args, BoolExpr::Kind const & kind,
                             bx_t const & identity, bx_t const & dominator)
    : state {State::infimum}
    , kind {kind}
    , identity {identity}
    , dominator {dominator}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
LatticeArgSet::insert(bx_t const & arg)
{
    switch (state) {

        case State::infimum:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (ARE_SAME(arg, dominator) || (IS_LIT(arg) && (args.find(~arg) != args.cend()))) {
                state = State::supremum;
            }
            else if (IS_LOG(arg)) {
                state = State::islog;
            }
            else if (arg->kind == kind) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
            }
            else if (!ARE_SAME(arg, identity)) {
                args.insert(arg);
                state = State::basic;
            }
            break;

        case State::basic:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (ARE_SAME(arg, dominator) || (IS_LIT(arg) && (args.find(~arg) != args.cend()))) {
                state = State::supremum;
            }
            else if (IS_LOG(arg)) {
                state = State::islog;
            }
            else if (arg->kind == kind) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
            }
            else if (!ARE_SAME(arg, identity)) {
                args.insert(arg);
            }
            break;

        case State::islog:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (ARE_SAME(arg, dominator) || (IS_LIT(arg) && (args.find(~arg) != args.cend()))) {
                state = State::supremum;
            }
            else if (arg->kind == kind) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
            }
            else if (!ARE_SAME(arg, identity)) {
                args.insert(arg);
            }
            break;

        case State::supremum:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (arg->kind == kind) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
            }
            break;

        case State::isill:
            break;
    }
}


bx_t
LatticeArgSet::reduce() const
{
    if (state == State::infimum)
        return identity;
    if (state == State::islog)
        return logical();
    if (state == State::supremum)
        return dominator;
    if (state == State::isill)
        return illogical();

    if (args.size() == 1) return *args.cbegin();

    return to_op();
}


OrArgSet::OrArgSet(vector<bx_t> const & args)
    : LatticeArgSet(args, BoolExpr::OR, Or::identity(), Or::dominator())
{}


bx_t
OrArgSet::to_op() const
{
    return std::make_shared<Or>(true, vector<bx_t>(args.cbegin(), args.cend()));
}


AndArgSet::AndArgSet(vector<bx_t> const & args)
    : LatticeArgSet(args, BoolExpr::AND, And::identity(), And::dominator())
{}


bx_t
AndArgSet::to_op() const
{
    return std::make_shared<And>(true, vector<bx_t>(args.cbegin(), args.cend()));
}


XorArgSet::XorArgSet(vector<bx_t> const & args)
    : state {State::basic}
    , parity {true}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
XorArgSet::insert(bx_t const & arg)
{
    switch (state) {

        case State::basic:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (IS_LOG(arg)) {
                state = State::islog;
            }
            else if (IS_KNOWN(arg)) {
                parity ^= static_cast<bool>(arg->kind);
            }
            // xor(x, y, z, z) <=> xor(x, y) ; xnor(x, y, z, z) <=> xnor(x, y)
            else if (args.find(arg) != args.cend()) {
                args.erase(arg);
            }
            // xor(x, y, z, ~z) <=> xnor(x, y) ; xnor(x, y, z, ~z) <=> xor(x, y)
            else if (IS_LIT(arg) && args.find(~arg) != args.cend()) {
                args.erase(~arg);
                parity ^= true;
            }
            // xor(x, xor(y, z)) <=> xor(x, y, z) ; xnor(x, xor(y, z)) <=> xnor(x, y, z)
            else if (IS_XOR(arg)) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
            }
            // xor(x, xnor(y, z)) <=> xnor(x, y, z) ; xnor(x, xnor(y, z)) <=> xor(x, y, z)
            else if (IS_XNOR(arg)) {
                auto op = std::static_pointer_cast<Operator const>(arg);
                for (bx_t const & _arg : op->args)
                    insert(_arg);
                parity ^= true;
            }
            else {
                args.insert(arg);
            }
            break;

        case State::islog:
            if (IS_ILL(arg))
                state = State::isill;
            break;

        case State::isill:
            break;
    }
}


bx_t
XorArgSet::to_op() const
{
    return std::make_shared<Xor>(true, vector<bx_t>(args.cbegin(), args.cend()));
}


bx_t
XorArgSet::reduce() const
{
    if (state == State::islog)
        return logical();
    if (state == State::isill)
        return illogical();

    bx_t y;

    if (args.size() == 0)
        y = zero();
    else if (args.size() == 1)
        y = *args.cbegin();
    else
        y = to_op();

    return parity ? y : ~y;
}


EqArgSet::EqArgSet(vector<bx_t> const & args)
    : state {State::basic}
    , has_zero {false}
    , has_one {false}
{
    for (bx_t const & arg : args) insert(arg->simplify());
}


void
EqArgSet::insert(bx_t const & arg)
{
    switch (state) {

        case State::basic:
            if (IS_ILL(arg)) {
                state = State::isill;
            }
            else if (IS_LOG(arg)) {
                state = State::islog;
            }
            else if (IS_ZERO(arg)) {
                has_zero = true;
                if (has_one)
                    args.clear();
            }
            else if (IS_ONE(arg)) {
                has_one = true;
                if (has_zero)
                    args.clear();
            }
            else if (IS_LIT(arg) && (args.find(~arg) != args.cend())) {
                has_zero = true;
                has_one = true;
                args.clear();
            }
            else {
                args.insert(arg);
            }
            break;

        case State::islog:
            if (IS_ILL(arg))
                state = State::isill;
            break;

        case State::isill:
            break;
    }
}


bx_t
EqArgSet::to_op() const
{
    return std::make_shared<Equal>(true, vector<bx_t>(args.cbegin(), args.cend()));
}


bx_t
EqArgSet::reduce() const
{
    if (state == State::islog)
        return logical();
    if (state == State::isill)
        return illogical();

    // eq(0, 1) <=> 0
    if (has_zero && has_one)
        return zero();

    // eq() <=> eq(0) <=> eq(1) <=> 1
    if ((static_cast<size_t>(has_zero) + static_cast<size_t>(has_one) + args.size()) < 2)
        return one();

    // eq(0, x, y) <=> nor(x, y)
    if (has_zero)
        return nor_s(vector<bx_t>(args.cbegin(), args.cend()));

    // eq(1, x, y) <=> x & y
    if (has_one)
        return and_s(vector<bx_t>(args.cbegin(), args.cend()));

    return to_op();
}
