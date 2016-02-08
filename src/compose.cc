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
Constant::_compose(const bx_t& self, const var2bx_t&) const
{
    return self;
}


bx_t
Complement::_compose(const bx_t& self, const var2bx_t& var2bx) const
{
    auto x = std::static_pointer_cast<Variable>(~self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : ~(search->second);
}


bx_t
Variable::_compose(const bx_t& self, const var2bx_t& var2bx) const
{
    auto x = std::static_pointer_cast<Variable>(self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : search->second;
}


bx_t
Operator::_compose(const bx_t& self, const var2bx_t& var2bx) const
{
    auto op = std::static_pointer_cast<Operator>(self);
    auto f = [&var2bx] (const bx_t& bx) { return compose(bx, var2bx); };
    return transform(op, f);
}


bx_t
boolexpr::compose(const bx_t& self, const var2bx_t& var2bx)
{
    return self->_compose(self, var2bx);
}


bx_t
Constant::_restrict(const bx_t& self, const point_t&) const
{
    return self;
}


bx_t
Complement::_restrict(const bx_t& self, const point_t& point) const
{
    auto x = std::static_pointer_cast<Variable>(~self);
    auto search = point.find(x);
    return (search == point.end()) ? self : ~(search->second);
}


bx_t
Variable::_restrict(const bx_t& self, const point_t& point) const
{
    auto x = std::static_pointer_cast<Variable>(self);
    auto search = point.find(x);
    return (search == point.end()) ? self : search->second;
}


bx_t
Operator::_restrict(const bx_t& self, const point_t& point) const
{
    auto op = std::static_pointer_cast<Operator>(self);
    auto f = [&point] (const bx_t& bx) { return restrict_(bx, point); };
    return simplify(transform(op, f));
}


bx_t
boolexpr::restrict_(const bx_t& self, const point_t& point)
{
    return self->_restrict(self, point);
}
