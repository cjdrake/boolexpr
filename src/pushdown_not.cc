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
Atom::_pushdown_not(const bx_t& self) const
{
    return self;
}


bx_t
Nor::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<Nor>(self);

    // ~(x0 | x1 | ...) <=> ~x0 & ~x1 & ...
    vector<bx_t> _args;
    for (const bx_t& arg : nop->args)
        _args.push_back(pushdown_not(~arg));

    return and_(std::move(_args));
}


bx_t
Or::_pushdown_not(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), pushdown_not);
}


bx_t
Nand::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<Nand>(self);

    // ~(x0 & x1 & ...) <=> ~x0 | ~x1 | ...
    vector<bx_t> _args;
    for (const bx_t& arg : nop->args)
        _args.push_back(pushdown_not(~arg));

    return or_(std::move(_args));
}


bx_t
And::_pushdown_not(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), pushdown_not);
}


bx_t
Xnor::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<Xnor>(self);

    // ~(x0 ^ x1 ^ x2 ^ ...) <=> ~x0 ^ x1 ^ x2 ^ ...
    vector<bx_t> _args {~nop->args[0]};
    for (auto it = nop->args.begin() + 1; it != nop->args.end(); ++it)
        _args.push_back(pushdown_not(*it));

    return xor_(std::move(_args));
}


bx_t
Xor::_pushdown_not(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), pushdown_not);
}


bx_t
Unequal::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<Unequal>(self);

    // ~eq(x0, x1, x2, ...) <=> eq(~x0, x1, x2, ...)
    vector<bx_t> _args {~nop->args[0]};
    for (auto it = nop->args.begin() + 1; it != nop->args.end(); ++it)
        _args.push_back(pushdown_not(*it));

    return eq(std::move(_args));
}


bx_t
Equal::_pushdown_not(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), pushdown_not);
}


bx_t
NotImplies::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<NotImplies>(self);

    // ~(p => q) <=> p & ~q
    auto p = pushdown_not(nop->args[0]);
    auto qn = pushdown_not(~nop->args[1]);

    return p & qn;
}


bx_t
Implies::_pushdown_not(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Implies>(self);

    // p => q <=> ~p | q
    auto pn = pushdown_not(~op->args[0]);
    auto q = pushdown_not(op->args[1]);

    return pn | q;
}


bx_t
NotIfThenElse::_pushdown_not(const bx_t& self) const
{
    auto nop = std::static_pointer_cast<NotIfThenElse>(self);

    // ~(s ? d1 : d0) <=> s ? ~d1 : ~d0
    auto s = pushdown_not(nop->args[0]);
    auto d1n = pushdown_not(~nop->args[1]);
    auto d0n = pushdown_not(~nop->args[2]);

    return ite(s, d1n, d0n);
}


bx_t
IfThenElse::_pushdown_not(const bx_t& self) const
{
    return transform(std::static_pointer_cast<Operator>(self), pushdown_not);
}


bx_t
boolexpr::pushdown_not(const bx_t& self)
{
    return self->_pushdown_not(self);
}
