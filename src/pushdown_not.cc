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
Atom::pushdown_not() const
{
    return shared_from_this();
}


static op_t
_op_pushdown_not(Operator const * const op)
{
    return op->transform([](bx_t const & arg){return arg->pushdown_not();});
}

bx_t Or::pushdown_not() const { return _op_pushdown_not(this); }
bx_t And::pushdown_not() const { return _op_pushdown_not(this); }
bx_t Xor::pushdown_not() const { return _op_pushdown_not(this); }
bx_t Equal::pushdown_not() const { return _op_pushdown_not(this); }
bx_t IfThenElse::pushdown_not() const { return _op_pushdown_not(this); }


bx_t
Nor::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Nor const>(self);

    // ~(x0 | x1 | ...) <=> ~x0 & ~x1 & ...
    vector<bx_t> _args;
    for (bx_t const & arg : nop->args)
        _args.push_back((~arg)->pushdown_not());

    return and_(std::move(_args));
}


bx_t
Nand::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Nand const>(self);

    // ~(x0 & x1 & ...) <=> ~x0 | ~x1 | ...
    vector<bx_t> _args;
    for (bx_t const & arg : nop->args)
        _args.push_back((~arg)->pushdown_not());

    return or_(std::move(_args));
}


bx_t
Xnor::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Xnor const>(self);

    // ~(x0 ^ x1 ^ x2 ^ ...) <=> ~x0 ^ x1 ^ x2 ^ ...
    vector<bx_t> _args {~nop->args[0]};
    for (auto it = nop->args.cbegin() + 1; it != nop->args.cend(); ++it)
        _args.push_back((*it)->pushdown_not());

    return xor_(std::move(_args));
}


bx_t
Unequal::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Unequal const>(self);

    // ~eq(x0, x1, x2, ...) <=> eq(~x0, x1, x2, ...)
    vector<bx_t> _args {~nop->args[0]};
    for (auto it = nop->args.cbegin() + 1; it != nop->args.cend(); ++it)
        _args.push_back((*it)->pushdown_not());

    return eq(std::move(_args));
}


bx_t
NotImplies::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<NotImplies const>(self);

    // ~(p => q) <=> p & ~q
    auto p = (nop->args[0])->pushdown_not();
    auto qn = (~nop->args[1])->pushdown_not();

    return p & qn;
}


bx_t
Implies::pushdown_not() const
{
    auto self = shared_from_this();
    auto op = std::static_pointer_cast<Implies const>(self);

    // p => q <=> ~p | q
    auto pn = (~op->args[0])->pushdown_not();
    auto q = (op->args[1])->pushdown_not();

    return pn | q;
}


bx_t
NotIfThenElse::pushdown_not() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<NotIfThenElse const>(self);

    // ~(s ? d1 : d0) <=> s ? ~d1 : ~d0
    auto s = (nop->args[0])->pushdown_not();
    auto d1n = (~nop->args[1])->pushdown_not();
    auto d0n = (~nop->args[2])->pushdown_not();

    return ite(s, d1n, d0n);
}
