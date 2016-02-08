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

#include "boolexpr.h"


using namespace boolexpr;


bx_t
Atom::_to_binop(const bx_t& self) const {
    return self;
}


bx_t Nor::_to_binop(const bx_t& self) const { return ~to_binop(~self); }
bx_t Nand::_to_binop(const bx_t& self) const { return ~to_binop(~self); }
bx_t Xnor::_to_binop(const bx_t& self) const { return ~to_binop(~self); }
bx_t Unequal::_to_binop(const bx_t& self) const { return ~to_binop(~self); }
bx_t NotImplies::_to_binop(const bx_t& self) const { return ~to_binop(~self); }
bx_t NotIfThenElse::_to_binop(const bx_t& self) const { return ~to_binop(~self); }


bx_t
Or::_to_binop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Operator>(self);

    if (op->args.size() == 0)  // LCOV_EXCL_LINE
        return Or::identity(); // LCOV_EXCL_LINE

    if (op->args.size() == 1)         // LCOV_EXCL_LINE
        return to_binop(op->args[0]); // LCOV_EXCL_LINE

    if (op->args.size() == 2)
        return transform(op, to_binop);

    // x0 | x1 | x2 | x3 <=> (x0 | x1) | (x2 | x3)
    size_t const mid = op->args.size() / 2;

    auto lo = or_(vector<bx_t>(op->args.begin(), op->args.begin() + mid));
    auto hi = or_(vector<bx_t>(op->args.begin() + mid, op->args.end()));

    return to_binop(lo) | to_binop(hi);
}


bx_t
And::_to_binop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Operator>(self);

    if (op->args.size() == 0)   // LCOV_EXCL_LINE
        return And::identity(); // LCOV_EXCL_LINE

    if (op->args.size() == 1)         // LCOV_EXCL_LINE
        return to_binop(op->args[0]); // LCOV_EXCL_LINE

    if (op->args.size() == 2)
        return transform(op, to_binop);

    // x0 & x1 & x2 & x3 <=> (x0 & x1) & (x2 & x3)
    size_t const mid = op->args.size() / 2;

    auto lo = and_(vector<bx_t>(op->args.begin(), op->args.begin() + mid));
    auto hi = and_(vector<bx_t>(op->args.begin() + mid, op->args.end()));

    return to_binop(lo) & to_binop(hi);
}


bx_t
Xor::_to_binop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Operator>(self);

    if (op->args.size() == 0)   // LCOV_EXCL_LINE
        return Xor::identity(); // LCOV_EXCL_LINE

    if (op->args.size() == 1)         // LCOV_EXCL_LINE
        return to_binop(op->args[0]); // LCOV_EXCL_LINE

    if (op->args.size() == 2)
        return transform(op, to_binop);

    // x0 ^ x1 ^ x2 ^ x3 <=> (x0 ^ x1) ^ (x2 ^ x3)
    size_t const mid = op->args.size() / 2;

    auto lo = xor_(vector<bx_t>(op->args.begin(), op->args.begin() + mid));
    auto hi = xor_(vector<bx_t>(op->args.begin() + mid, op->args.end()));

    return to_binop(lo) ^ to_binop(hi);
}


bx_t
Equal::_to_binop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Operator>(self);

    if (op->args.size() < 2) // LCOV_EXCL_LINE
        return one();        // LCOV_EXCL_LINE

    if (op->args.size() == 2)
        return transform(op, to_binop);

    vector<bx_t> _args;
    for (const bx_t& arg : op->args)
        _args.push_back(to_binop(arg));

    vector<bx_t> pairs;
    for (size_t i = 0; i < (_args.size() - 1); ++i) {
        for (size_t j = i + 1; j < _args.size(); ++j)
            pairs.push_back(eq({_args[i], _args[j]}));
    }

    return and_(pairs);
}


bx_t
Implies::_to_binop(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), to_binop);
}


bx_t
IfThenElse::_to_binop(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), to_binop);
}


bx_t
boolexpr::to_binop(const bx_t& self)
{
    return self->_to_binop(self);
}
