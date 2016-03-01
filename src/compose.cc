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
Constant::compose(const var2bx_t&) const
{
    auto self = shared_from_this();
    return std::static_pointer_cast<const BoolExpr>(self);
}


bx_t
Complement::compose(const var2bx_t& var2bx) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<const Variable>(~self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : ~(search->second);
}


bx_t
Variable::compose(const var2bx_t& var2bx) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<const Variable>(self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : search->second;
}


bx_t
Operator::compose(const var2bx_t& var2bx) const
{
    auto self = shared_from_this();
    auto op = std::static_pointer_cast<const Operator>(self);
    auto f = [&var2bx] (const bx_t& bx) { return bx->compose(var2bx); };
    return transform(op, f);
}


bx_t
Constant::restrict_(const point_t&) const
{
    auto self = shared_from_this();
    return std::static_pointer_cast<const BoolExpr>(self);
}


bx_t
Complement::restrict_(const point_t& point) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<const Variable>(~self);
    auto search = point.find(x);
    return (search == point.end()) ? self : ~(search->second);
}


bx_t
Variable::restrict_(const point_t& point) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<const Variable>(self);
    auto search = point.find(x);
    return (search == point.end()) ? self : search->second;
}


bx_t
Operator::restrict_(const point_t& point) const
{
    auto self = shared_from_this();
    auto op = std::static_pointer_cast<const Operator>(self);
    auto f = [&point] (const bx_t& bx) { return bx->restrict_(point); };
    return simplify(transform(op, f));
}
