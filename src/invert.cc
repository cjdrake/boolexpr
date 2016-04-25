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


bx_t Zero::invert() const { return one(); }
bx_t One::invert()  const { return zero(); }


bx_t
Logical::invert() const
{
    return shared_from_this();
}


bx_t
Illogical::invert() const
{
    return shared_from_this();
}


bx_t
Complement::invert() const
{
    auto self = shared_from_this();
    auto xn = std::static_pointer_cast<Complement const>(self);
    return xn->ctx->get_lit(xn->id + 1);
}


bx_t
Variable::invert() const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    return x->ctx->get_lit(x->id - 1);
}


bx_t
Nor::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Nor const>(self);
    return std::make_shared<Or>(nop->simple, nop->args);
}


bx_t
Or::invert() const
{
    return std::make_shared<Nor>(simple, args);
}


bx_t
Nand::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Nand const>(self);
    return std::make_shared<And>(nop->simple, nop->args);
}


bx_t
And::invert() const
{
    return std::make_shared<Nand>(simple, args);
}


bx_t
Xnor::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Xnor const>(self);
    return std::make_shared<Xor>(nop->simple, nop->args);
}


bx_t
Xor::invert() const
{
    return std::make_shared<Xnor>(simple, args);
}


bx_t
Unequal::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<Unequal const>(self);
    return std::make_shared<Equal>(nop->simple, nop->args);
}


bx_t
Equal::invert() const
{
    return std::make_shared<Unequal>(simple, args);
}


bx_t
NotImplies::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<NotImplies const>(self);
    return std::make_shared<Implies>(nop->simple, nop->args[0], nop->args[1]);
}


bx_t
Implies::invert() const
{
    return std::make_shared<NotImplies>(simple, args[0], args[1]);
}


bx_t
NotIfThenElse::invert() const
{
    auto self = shared_from_this();
    auto nop = std::static_pointer_cast<NotIfThenElse const>(self);
    return std::make_shared<IfThenElse>(nop->simple, nop->args[0], nop->args[1], nop->args[2]);
}


bx_t
IfThenElse::invert() const
{
    return std::make_shared<NotIfThenElse>(simple, args[0], args[1], args[2]);
}
