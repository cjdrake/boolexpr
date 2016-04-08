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


Context::Context() : id {0} {}


var_t
Context::get_var(string name)
{
    auto search = vars.find(name);
    if (search == vars.end()) {
        auto xn = std::make_shared<Complement>(this, id++);
        auto x = std::make_shared<Variable>(this, id++);
        vars.insert({name, x});
        id2name.insert({xn->id >> 1, name});
        id2lit.insert({xn->id, xn});
        id2lit.insert({x->id, x});
        return x;
    }
    return search->second;
}


string
Context::get_name(id_t id) const
{
    return id2name.find(id >> 1)->second;
}


lit_t
Context::get_lit(id_t id) const
{
    return id2lit.find(id)->second;
}
