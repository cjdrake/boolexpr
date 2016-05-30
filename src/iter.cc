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


dfs_iter::dfs_iter()
    : p {nullptr}
{}


dfs_iter::dfs_iter(bx_t const & start)
{
    stack.push_back(start);
    colors.insert({start, Color::WHITE});
    advance_one();
}


void
dfs_iter::advance_one()
{
    while (stack.size() > 0) {
        bx_t const & item = stack.back();
        auto color = colors.find(item)->second;

        if (color == Color::WHITE) {
            if (IS_ATOM(item)) {
                colors[item] = Color::BLACK;
                stack.pop_back();
                p = &item;
                return;
            }
            else {
                colors[item] = Color::GRAY;
                auto op = std::static_pointer_cast<Operator const>(item);
                for (auto it = op->args.crbegin(); it != op->args.crend(); ++it) {
                    stack.push_back(*it);
                    colors.insert({*it, Color::WHITE});
                }
            }
        }
        else if (color == Color::GRAY) {
            colors[item] = Color::BLACK;
            stack.pop_back();
            p = &item;
            return;
        }
        else {
            stack.pop_back();
        }
    }

    p = nullptr;
}


bool
dfs_iter::operator==(dfs_iter const & rhs) const
{ return p == rhs.p; }


bool
dfs_iter::operator!=(dfs_iter const & rhs) const
{ return !(*this == rhs); }


bx_t const &
dfs_iter::operator*() const
{ return *p; }


dfs_iter const &
dfs_iter::operator++()
{
    advance_one();
    return *this;
}


space_iter::space_iter()
    : n {0}
{ counter.push_back(true); }


space_iter::space_iter(size_t n)
    : n {n}
{
    for (size_t i = 0; i <= n; ++i)
        counter.push_back(false);
}


bool
space_iter::operator==(space_iter const & rhs) const
{ return counter[n] == rhs.counter[rhs.n]; }


bool
space_iter::operator!=(space_iter const & rhs) const
{ return !(*this == rhs); }


vector<bool> const &
space_iter::operator*() const
{ return counter; }


space_iter const &
space_iter::operator++()
{
    // Increment the counter
    for (size_t i = 0; i <= n; ++i)
        if ((counter[i] = (counter[i] != true)))
            break;

    return *this;
}


bool
space_iter::parity() const
{
    auto val = false;
    for (size_t i = 0; i < n; ++i)
        val ^= counter[i];
    return val;
}


points_iter::points_iter()
    : it {space_iter()}
{}


points_iter::points_iter(vector<var_t> const & vars)
    : it {space_iter(vars.size())}, vars {vars}
{
    for (var_t const & x : vars)
        point.insert({x, zero()});
}


bool
points_iter::operator==(points_iter const & rhs) const
{ return it == rhs.it; }


bool
points_iter::operator!=(points_iter const & rhs) const
{ return !(*this == rhs); }


point_t const &
points_iter::operator*() const
{ return point; }


points_iter const &
points_iter::operator++()
{
    point.clear();

    ++it;

    for (size_t i = 0; i < vars.size(); ++i) {
        if ((*it)[i])
            point.insert({vars[i], one()});
        else
            point.insert({vars[i], zero()});
    }

    return *this;
}


domain_iter::domain_iter()
    : it {points_iter()}
{}


domain_iter::domain_iter(bx_t const & f)
    : s {f->support()}, it {vector<var_t>(s.begin(), s.end())}
{}


bool
domain_iter::operator==(domain_iter const & rhs) const
{ return it == rhs.it; }


bool
domain_iter::operator!=(domain_iter const & rhs) const
{ return !(*this == rhs); }


point_t const &
domain_iter::operator*() const
{ return *it; }


domain_iter const &
domain_iter::operator++()
{
    ++it;
    return *this;
}


cf_iter::cf_iter()
    : it {points_iter()}
{}


cf_iter::cf_iter(bx_t const & f, vector<var_t> const & vars)
    : f {f}, it {points_iter(vars)}, cf {f->restrict_(*it)}
{}


bool
cf_iter::operator==(cf_iter const & rhs) const
{ return it == rhs.it; }


bool
cf_iter::operator!=(cf_iter const & rhs) const
{ return !(*this == rhs); }


bx_t const &
cf_iter::operator*() const
{ return cf; }


cf_iter const &
cf_iter::operator++()
{
    ++it;
    cf = f->restrict_(*it);
    return *this;
}
