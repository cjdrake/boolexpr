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


// Atoms are already simple
bx_t
Atom::_simplify(const bx_t& self) const
{
    return self;
}


// Use the positive form to simplify inverted operators
static bx_t
_nop_simplify(const bx_t& self)
{
    auto nop = std::static_pointer_cast<Operator>(self);
    if (nop->simple) return nop;
    return ~simplify(~nop);
}

bx_t Nor::_simplify(const bx_t& self) const { return _nop_simplify(self); }
bx_t Nand::_simplify(const bx_t& self) const { return _nop_simplify(self); }
bx_t Xnor::_simplify(const bx_t& self) const { return _nop_simplify(self); }
bx_t Unequal::_simplify(const bx_t& self) const { return _nop_simplify(self); }
bx_t NotImplies::_simplify(const bx_t& self) const { return _nop_simplify(self); }
bx_t NotIfThenElse::_simplify(const bx_t& self) const { return _nop_simplify(self); }


bx_t
Or::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Or>(self);
    if (op->simple) return op;
    return OrArgSet(op->args).reduce();
}


bx_t
And::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<And>(self);
    if (op->simple) return op;
    return AndArgSet(op->args).reduce();
}


bx_t
Xor::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Xor>(self);
    if (op->simple) return op;
    return XorArgSet(op->args).reduce();
}


bx_t
Equal::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Equal>(self);
    if (op->simple) return op;
    return EqArgSet(op->args).reduce();
}


bx_t
Implies::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<Implies>(self);

    if (op->simple) return op;

    auto p = simplify(op->args[0]);
    auto q = simplify(op->args[1]);

    // 0 => q <=> p => 1 <=> 1
    if (IS_ZERO(p) || IS_ONE(q)) return one();

    // 1 => q <=> q
    if (IS_ONE(p)) return q;

    // p => 0 <=> ~p
    if (IS_ZERO(q)) return ~p;

    // q => q <=> 1
    if (p == q) return one();

    // ~q => q <=> q
    if (IS_LIT(p) && IS_LIT(q) && (p == ~q)) return q;

    return std::make_shared<Implies>(true, p, q);
}


bx_t
IfThenElse::_simplify(const bx_t& self) const
{
    auto op = std::static_pointer_cast<IfThenElse>(self);

    if (op->simple) return op;

    auto s = simplify(op->args[0]);
    auto d1 = simplify(op->args[1]);
    auto d0 = simplify(op->args[2]);

    // 0 ? d1 : d0 <=> d0
    if (IS_ZERO(s)) return d0;
    // 1 ? d1 : d0 <=> d1
    if (IS_ONE(s)) return d1;

    if (IS_ZERO(d1)) {
        // s ? 0 : 0 <=> 0
        if (IS_ZERO(d0)) return zero();
        // s ? 0 : 1 <=> ~s
        if (IS_ONE(d0)) return ~s;
        // s ? 0 : d0 <=> ~s & d0
        return simplify(~s & d0);
    }

    if (IS_ONE(d1)) {
        // s ? 1 : 0 <=> s
        if (IS_ZERO(d0)) return s;
        // s ? 1 : 1 <=> 1
        if (IS_ONE(d0)) return one();
        // s ? 1 : d0 <=> s | d0
        return simplify(s | d0);
    }

    // s ? d1 : 0 <=> s & d1
    if (IS_ZERO(d0)) return simplify(s & d1);
    // s ? d1 : 1 <=> ~s | d1
    if (IS_ONE(d0)) return simplify(~s | d1);
    // s ? d1 : d1 <=> d1
    if (d1 == d0) return d1;
    // s ? s : d0 <=> s | d0
    if (s == d1) return simplify(s | d0);
    // s ? d1 : s <=> s & d1
    if (s == d0) return simplify(s & d1);

    return std::make_shared<IfThenElse>(true, s, d1, d0);
}


bx_t
boolexpr::simplify(const bx_t& self)
{
    return self->_simplify(self);
}
