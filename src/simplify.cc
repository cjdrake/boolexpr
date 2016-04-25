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


// Atoms are already simple
bx_t
Atom::simplify() const
{
    return shared_from_this();
}


bx_t
Operator::simplify() const
{
    if (simple)
        return shared_from_this();

    return _simplify();
}


bx_t
Nor::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
Or::_simplify() const
{
    return OrArgSet(args).reduce();
}


bx_t
Nand::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
And::_simplify() const
{
    return AndArgSet(args).reduce();
}


bx_t
Xnor::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
Xor::_simplify() const
{
    return XorArgSet(args).reduce();
}


bx_t
Unequal::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
Equal::_simplify() const
{
    return EqArgSet(args).reduce();
}


bx_t
NotImplies::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
Implies::_simplify() const
{
    auto p = args[0]->simplify();
    auto q = args[1]->simplify();

    if (IS_ILL(p) || IS_ILL(q))
        return illogical();

    // 0 => q <=> p => 1 <=> 1
    if (IS_ZERO(p) || IS_ONE(q))
        return one();

    // 1 => q <=> q
    if (IS_ONE(p))
        return q;

    // p => 0 <=> ~p
    if (IS_ZERO(q))
        return ~p;

    // X => q <=> p => X <=> X
    if (IS_LOG(p) || IS_LOG(q))
        return logical();

    // q => q <=> 1
    if (p == q)
        return one();

    // ~q => q <=> q
    if (IS_LIT(p) && IS_LIT(q) && (p == ~q))
        return q;

    return std::make_shared<Implies>(true, p, q);
}


bx_t
NotIfThenElse::_simplify() const
{
    auto op = ~shared_from_this();
    return ~op->simplify();
}


bx_t
IfThenElse::_simplify() const
{
    auto s = args[0]->simplify();
    auto d1 = args[1]->simplify();
    auto d0 = args[2]->simplify();

    if (IS_ILL(s) || IS_ILL(d1) || IS_ILL(d0))
        return illogical();

    // 0 ? d1 : d0 <=> d0
    if (IS_ZERO(s))
        return d0;
    // 1 ? d1 : d0 <=> d1
    if (IS_ONE(s))
        return d1;

    if (IS_ZERO(d1)) {
        // s ? 0 : 0 <=> 0
        if (IS_ZERO(d0))
            return zero();
        // s ? 0 : 1 <=> ~s
        if (IS_ONE(d0))
            return ~s;
        // s ? 0 : d0 <=> ~s & d0
        return and_s({~s, d0});
    }

    if (IS_ONE(d1)) {
        // s ? 1 : 0 <=> s
        if (IS_ZERO(d0))
            return s;
        // s ? 1 : 1 <=> 1
        if (IS_ONE(d0))
            return one();
        // s ? 1 : d0 <=> s | d0
        return or_s({s, d0});
    }

    // s ? d1 : 0 <=> s & d1
    if (IS_ZERO(d0))
        return and_s({s, d1});
    // s ? d1 : 1 <=> ~s | d1
    if (IS_ONE(d0))
        return or_s({~s, d1});

    // (s ? X : d0) <=> (s ? d1 : X) <=> X
    if (IS_LOG(d1) || IS_LOG(d0))
        return logical();

    // s ? d1 : d1 <=> d1
    if (d1 == d0)
        return d1;

    // X ? d1 : d0 <=> X
    // NOTE: If you can prove d0 <=> d1, then should return d0.
    //       But that proof is too expensive to gate this operation.
    if (IS_LOG(s))
        return logical();

    // s ? s : d0 <=> s | d0
    if (s == d1)
        return or_s({s, d0});
    // s ? d1 : s <=> s & d1
    if (s == d0)
        return and_s({s, d1});

    return std::make_shared<IfThenElse>(true, s, d1, d0);
}
