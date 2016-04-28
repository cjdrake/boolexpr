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
Atom::to_posop() const
{
    return shared_from_this();
}


bx_t
Nor::to_posop() const
{
    // ~(x0 | x1 | ...) <=> ~x0 & ~x1 & ...
    size_t n = args.size();
    vector<bx_t> _args(n);

    for (size_t i = 0; i < n; ++i)
        _args[i] = (~args[i])->to_posop();

    return and_(std::move(_args));
}


bx_t Or::to_posop() const
{
    return transform([](bx_t const & arg){return arg->to_posop();});
}


bx_t
Nand::to_posop() const
{
    // ~(x0 & x1 & ...) <=> ~x0 | ~x1 | ...
    size_t n = args.size();
    vector<bx_t> _args(n);

    for (size_t i = 0; i < n; ++i)
        _args[i] = (~args[i])->to_posop();

    return or_(std::move(_args));
}


bx_t And::to_posop() const
{
    return transform([](bx_t const & arg){return arg->to_posop();});
}


bx_t
Xnor::to_posop() const
{
    // ~(x0 ^ x1 ^ x2 ^ ...) <=> ~x0 ^ x1 ^ x2 ^ ...
    size_t n = args.size();
    vector<bx_t> _args(n);

    _args[0] = (~args[0])->to_posop();
    for (size_t i = 1; i < n; ++i)
        _args[i] = args[i]->to_posop();

    return xor_(std::move(_args));
}


bx_t Xor::to_posop() const
{
    return transform([](bx_t const & arg){return arg->to_posop();});
}


bx_t
Unequal::to_posop() const
{
    // ~eq(x0, x1, x2, ...) <=> eq(~x0, x1, x2, ...)
    size_t n = args.size();
    vector<bx_t> _args(n);

    _args[0] = (~args[0])->to_posop();
    for (size_t i = 1; i < n; ++i)
        _args[i] = args[i]->to_posop();

    return eq(std::move(_args));
}


bx_t Equal::to_posop() const
{
    return transform([](bx_t const & arg){return arg->to_posop();});
}


bx_t
NotImplies::to_posop() const
{
    // ~(p => q) <=> p & ~q
    auto p = (args[0])->to_posop();
    auto qn = (~args[1])->to_posop();

    return p & qn;
}


bx_t
Implies::to_posop() const
{
    // p => q <=> ~p | q
    auto pn = (~args[0])->to_posop();
    auto q = (args[1])->to_posop();

    return pn | q;
}


bx_t IfThenElse::to_posop() const
{
    return transform([](bx_t const & arg){return arg->to_posop();});
}


bx_t
NotIfThenElse::to_posop() const
{
    // ~(s ? d1 : d0) <=> s ? ~d1 : ~d0
    auto s = (args[0])->to_posop();
    auto d1n = (~args[1])->to_posop();
    auto d0n = (~args[2])->to_posop();

    return ite(s, d1n, d0n);
}
