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


using namespace boolexpr;


bx_t
Atom::_to_latop(const bx_t& self) const {
    return self;
}


bx_t
LatticeOperator::_to_latop(const bx_t& self) const
{
    return transform(std::static_pointer_cast<const Operator>(self), to_latop);
}


bx_t Nor::_to_latop(const bx_t& self) const { return ~to_latop(~self); }
bx_t Nand::_to_latop(const bx_t& self) const { return ~to_latop(~self); }
bx_t Xnor::_to_latop(const bx_t& self) const { return ~to_latop(~self); }
bx_t Unequal::_to_latop(const bx_t& self) const { return ~to_latop(~self); }
bx_t NotImplies::_to_latop(const bx_t& self) const { return ~to_latop(~self); }
bx_t NotIfThenElse::_to_latop(const bx_t& self) const { return ~to_latop(~self); }


bx_t
Xor::_to_latop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<const Operator>(self);

    if (op->args.size() == 0)   // LCOV_EXCL_LINE
        return Xor::identity(); // LCOV_EXCL_LINE

    if (op->args.size() == 1)         // LCOV_EXCL_LINE
        return to_latop(op->args[0]); // LCOV_EXCL_LINE

    if (op->args.size() == 2) {
        // x0 ^ x1 <=> ~x0 & x1 | x0 & ~x1
        auto x0 = to_latop(op->args[0]);
        auto x1 = to_latop(op->args[1]);
        return (~x0 & x1) | (x0 & ~x1);
    }

    // x0 ^ x1 ^ x2 ^ x3 <=> (x0 ^ x1) ^ (x2 ^ x3)
    size_t const mid = op->args.size() / 2;

    auto lo = xor_(vector<bx_t>(op->args.begin(), op->args.begin() + mid));
    auto hi = xor_(vector<bx_t>(op->args.begin() + mid, op->args.end()));

    return to_latop(lo ^ hi);
}


bx_t
Equal::_to_latop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<const Operator>(self);

    // eq(x0, x1, x2) <=> ~x0 & ~x1 & ~x2 | x0 & x1 & x2
    vector<bx_t> xs, xns;

    for (const bx_t& arg : op->args) {
        auto x = to_latop(arg);
        xs.push_back(x);
        xns.push_back(~x);
    }

    return and_(std::move(xns)) | and_(std::move(xs));
}


bx_t
Implies::_to_latop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<const Implies>(self);

    auto p = to_latop(op->args[0]);
    auto q = to_latop(op->args[1]);

    return ~p | q;
}


bx_t
IfThenElse::_to_latop(const bx_t& self) const
{
    auto op = std::static_pointer_cast<const IfThenElse>(self);

    auto s = to_latop(op->args[0]);
    auto d1 = to_latop(op->args[1]);
    auto d0 = to_latop(op->args[2]);

    return (s & d1) | (~s & d0);
}


bx_t
boolexpr::to_latop(const bx_t& self)
{
    return self->_to_latop(self);
}


bx_t
boolexpr::to_nnf(const bx_t& self)
{
    return simplify(self->_to_latop(self)->pushdown_not());
}
