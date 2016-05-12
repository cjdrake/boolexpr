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


Array::Array(vector<bx_t> const & items)
    : items {items}
{}


Array::Array(vector<bx_t> const && items)
    : items {items}
{}


Array::Array(std::initializer_list<bx_t> const items)
    : items {items}
{}


Array
Array::compose(var2bx_t const & var2bx) const
{
    size_t n = this->items.size();
    vector<bx_t> items(n);

    for (size_t i = 0; i < n; ++i)
        items[i] = this->items[i]->compose(var2bx);

    return Array(std::move(items));
}


Array
Array::restrict_(point_t const & point) const
{
    size_t n = this->items.size();
    vector<bx_t> items(n);

    for (size_t i = 0; i < n; ++i)
        items[i] = this->items[i]->restrict_(point);

    return Array(std::move(items));
}


bx_t
Array::or_reduce() const
{
    return or_(items);
}


bx_t
Array::and_reduce() const
{
    return and_(items);
}


bx_t
Array::xor_reduce() const
{
    return xor_(items);
}
