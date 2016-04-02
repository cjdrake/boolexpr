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
Constant::compose(var2bx_t const &) const
{
    return shared_from_this();
}


bx_t
Complement::compose(var2bx_t const & var2bx) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(~self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : ~(search->second);
}


bx_t
Variable::compose(var2bx_t const & var2bx) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    auto search = var2bx.find(x);
    return (search == var2bx.end()) ? self : search->second;
}


bx_t
Operator::compose(var2bx_t const & var2bx) const
{
    return transform([&var2bx](bx_t const & bx){return bx->compose(var2bx);});
}


bx_t
Constant::restrict_(point_t const &) const
{
    return shared_from_this();
}


bx_t
Complement::restrict_(point_t const & point) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(~self);
    auto search = point.find(x);
    return (search == point.end()) ? self : ~(search->second);
}


bx_t
Variable::restrict_(point_t const & point) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    auto search = point.find(x);
    return (search == point.end()) ? self : search->second;
}


bx_t
Operator::restrict_(point_t const & point) const
{
    auto f = [&point] (bx_t const & bx) { return bx->restrict_(point); };
    return transform(f)->simplify();
}
