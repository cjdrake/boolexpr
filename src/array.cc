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


Array::Array()
    : items {}
{}


Array::Array(vector<bx_t> const & items)
    : items {items}
{}


Array::Array(vector<bx_t> const && items)
    : items {items}
{}


Array::Array(std::initializer_list<bx_t> const items)
    : items {items}
{}


Array *
boolexpr::operator~(Array const & self)
{
    size_t n = self.items.size();
    vector<bx_t> items(n);

    for (size_t i = 0; i < n; ++i)
        items[i] = ~self.items[i];

    return new Array(std::move(items));
}


Array *
boolexpr::operator|(Array const & lhs, Array const & rhs)
{
    vector<bx_t> items;

    auto lhs_it = lhs.items.begin();
    auto rhs_it = rhs.items.begin();

    while (lhs_it != lhs.items.end() && rhs_it != rhs.items.end())
        items.push_back(*lhs_it++ | *rhs_it++);

    while (lhs_it != lhs.items.end())
        items.push_back(*lhs_it++);

    while (rhs_it != rhs.items.end())
        items.push_back(*rhs_it++);

    return new Array(std::move(items));
}


Array *
boolexpr::operator&(Array const & lhs, Array const & rhs)
{
    vector<bx_t> items;

    auto lhs_it = lhs.items.begin();
    auto rhs_it = rhs.items.begin();

    while (lhs_it != lhs.items.end() && rhs_it != rhs.items.end())
        items.push_back(*lhs_it++ & *rhs_it++);

    while (lhs_it != lhs.items.end())
        items.push_back(*lhs_it++);

    while (rhs_it != rhs.items.end())
        items.push_back(*rhs_it++);

    return new Array(std::move(items));
}


Array *
boolexpr::operator^(Array const & lhs, Array const & rhs)
{
    vector<bx_t> items;

    auto lhs_it = lhs.items.begin();
    auto rhs_it = rhs.items.begin();

    while (lhs_it != lhs.items.end() && rhs_it != rhs.items.end())
        items.push_back(*lhs_it++ ^ *rhs_it++);

    while (lhs_it != lhs.items.end())
        items.push_back(*lhs_it++);

    while (rhs_it != rhs.items.end())
        items.push_back(*rhs_it++);

    return new Array(std::move(items));
}


Array *
boolexpr::operator+(Array const & lhs, Array const & rhs)
{
    vector<bx_t> items(lhs.items.size() + rhs.items.size());

    size_t cnt = 0;

    for (size_t i = 0; i < lhs.items.size(); ++i)
        items[cnt++] = lhs.items[i];

    for (size_t i = 0; i < rhs.items.size(); ++i)
        items[cnt++] = rhs.items[i];

    return new Array(std::move(items));
}


Array *
boolexpr::operator*(Array const & lhs, size_t num)
{
    vector<bx_t> items(num * lhs.items.size());

    size_t cnt = 0;

    for (size_t i = 0; i < num; ++i)
        for (size_t j = 0; j < lhs.items.size(); ++j)
            items[cnt++] = lhs.items[j];

    return new Array(std::move(items));
}


Array *
boolexpr::operator*(size_t num, Array const & rhs)
{
    vector<bx_t> items(num * rhs.items.size());

    size_t cnt = 0;

    for (size_t i = 0; i < num; ++i)
        for (size_t j = 0; j < rhs.items.size(); ++j)
            items[cnt++] = rhs.items[j];

    return new Array(std::move(items));
}


bx_t const &
Array::operator[](size_t index) const
{
    return this->items[index];
}


bx_t &
Array::operator[](size_t index)
{
    return this->items[index];
}


size_t
Array::size() const
{
    return this->items.size();
}


vector<bx_t>::const_iterator
Array::begin() const
{
    return this->items.begin();
}


vector<bx_t>::const_iterator
Array::end() const
{
    return this->items.end();
}


Array *
Array::compose(var2bx_t const & var2bx) const
{
    size_t n = this->items.size();
    vector<bx_t> items(n);

    for (size_t i = 0; i < n; ++i)
        items[i] = this->items[i]->compose(var2bx);

    return new Array(std::move(items));
}


Array *
Array::restrict_(point_t const & point) const
{
    size_t n = this->items.size();
    vector<bx_t> items(n);

    for (size_t i = 0; i < n; ++i)
        items[i] = this->items[i]->restrict_(point);

    return new Array(std::move(items));
}


bool
Array::equiv(Array const & other) const
{

    if (this->items.size() != other.items.size())
        return false;

    for (size_t i = 0; i < this->items.size(); ++i)
        if (!this->items[i]->equiv(other.items[i]))
            return false;

    return true;
}


Array *
Array::zext(size_t num) const
{
    vector<bx_t> items(this->items.size() + num);

    size_t cnt = 0;
    for (size_t i = 0; i < this->items.size(); ++i)
        items[cnt++] = this->items[i];

    for (size_t i = 0; i < num; ++i)
        items[cnt++] = zero();

    return new Array(std::move(items));
}


Array *
Array::sext(size_t num) const
{
    vector<bx_t> items(this->items.size() + num);

    size_t cnt = 0;
    for (size_t i = 0; i < this->items.size(); ++i)
        items[cnt++] = this->items[i];

    auto sign = this->items[this->items.size()-1];
    for (size_t i = 0; i < num; ++i)
        items[cnt++] = sign;

    return new Array(std::move(items));
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


std::pair<Array *, Array *>
Array::lsh(Array const & a) const
{
    auto m = this->items.size();
    auto n = a.items.size();

    assert(m >= n);

    vector<bx_t> left(m);
    vector<bx_t> right(n);

    for (size_t i = 0; i < n; ++i)
        left[i] = a.items[i];

    for (size_t i = n; i < m; ++i)
        left[i] = this->items[i-n];

    for (size_t i = 0; i < n; ++i)
        right[i] = this->items[i+m-n];

    return std::make_pair(new Array(std::move(left)),
                          new Array(std::move(right)));
}


std::pair<Array *, Array *>
Array::rsh(Array const & a) const
{
    auto m = this->items.size();
    auto n = a.items.size();

    assert(m >= n);

    vector<bx_t> left(n);
    vector<bx_t> right(m);

    for (size_t i = 0; i < n; ++i)
        left[i] = this->items[i];

    for (size_t i = 0; i < (m-n); ++i)
        right[i] = this->items[i+n];

    for (size_t i = (m-n); i < m; ++i)
        right[i] = a.items[i-m+n];

    return std::make_pair(new Array(std::move(left)),
                          new Array(std::move(right)));
}


std::pair<Array *, Array *>
Array::arsh(size_t n) const
{
    auto m = this->items.size();

    assert(m >= n);

    vector<bx_t> left(n);
    vector<bx_t> right(m);

    for (size_t i = 0; i < n; ++i)
        left[i] = this->items[i];

    for (size_t i = 0; i < (m-n); ++i)
        right[i] = this->items[i+n];

    for (size_t i = (m-n); i < m; ++i)
        right[i] = this->items[m-1];

    return std::make_pair(new Array(std::move(left)),
                          new Array(std::move(right)));
}
