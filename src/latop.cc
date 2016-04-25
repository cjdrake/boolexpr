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


using namespace boolexpr;


bx_t
Atom::to_latop() const
{
    return shared_from_this();
}


bx_t
LatticeOperator::to_latop() const
{
    return transform([](bx_t const & arg){return arg->to_latop();});
}


static bx_t
_nop_to_latop(BoolExpr const * const bx)
{
    auto op = ~bx->shared_from_this();
    return ~op->to_latop();
}

bx_t Nor::to_latop() const { return _nop_to_latop(this); }
bx_t Nand::to_latop() const { return _nop_to_latop(this); }
bx_t Xnor::to_latop() const { return _nop_to_latop(this); }
bx_t Unequal::to_latop() const { return _nop_to_latop(this); }
bx_t NotImplies::to_latop() const { return _nop_to_latop(this); }
bx_t NotIfThenElse::to_latop() const { return _nop_to_latop(this); }


bx_t
Xor::to_latop() const
{
    if (args.size() == 0)       // LCOV_EXCL_LINE
        return Xor::identity(); // LCOV_EXCL_LINE

    if (args.size() == 1)           // LCOV_EXCL_LINE
        return args[0]->to_latop(); // LCOV_EXCL_LINE

    if (args.size() == 2) {
        // x0 ^ x1 <=> ~x0 & x1 | x0 & ~x1
        auto x0 = args[0]->to_latop();
        auto x1 = args[1]->to_latop();
        return (~x0 & x1) | (x0 & ~x1);
    }

    // x0 ^ x1 ^ x2 ^ x3 <=> (x0 ^ x1) ^ (x2 ^ x3)
    size_t const mid = args.size() / 2;

    auto lo = xor_(vector<bx_t>(args.cbegin(), args.cbegin() + mid));
    auto hi = xor_(vector<bx_t>(args.cbegin() + mid, args.cend()));

    return (lo ^ hi)->to_latop();
}


bx_t
Equal::to_latop() const
{
    size_t n = args.size();

    // eq(x0, x1, x2) <=> ~x0 & ~x1 & ~x2 | x0 & x1 & x2
    vector<bx_t> xs(n), xns(n);

    for (size_t i = 0 ; i < n; ++i) {
        auto x = args[i]->to_latop();
        xs[i] = x;
        xns[i] = ~x;
    }

    return and_(std::move(xns)) | and_(std::move(xs));
}


bx_t
Implies::to_latop() const
{
    auto p = args[0]->to_latop();
    auto q = args[1]->to_latop();

    return ~p | q;
}


bx_t
IfThenElse::to_latop() const
{
    auto s = args[0]->to_latop();
    auto d1 = args[1]->to_latop();
    auto d0 = args[2]->to_latop();

    return (s & d1) | (~s & d0);
}
