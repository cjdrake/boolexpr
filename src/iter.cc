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


dfs_iter::dfs_iter() : p {nullptr} {}


dfs_iter::dfs_iter(const bx_t& start)
{
    stack.push_back(start);
    colors.insert({start, Color::WHITE});
    _advance_one();
}


void
dfs_iter::_advance_one()
{
    while (stack.size() > 0) {
        const bx_t& item = stack.back();
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
                auto op = std::static_pointer_cast<const Operator>(item);
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
dfs_iter::operator==(const dfs_iter& rhs) const
{
    return p == rhs.p;
}


bool
dfs_iter::operator!=(const dfs_iter& rhs) const
{
    return !(*this == rhs);
}


const bx_t&
dfs_iter::operator*() const
{
    return *p;
}


const dfs_iter&
dfs_iter::operator++()
{
    _advance_one();
    return *this;
}
