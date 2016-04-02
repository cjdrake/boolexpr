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


uint32_t
Atom::depth() const
{
    return 0;
}


uint32_t
Operator::depth() const
{
    uint32_t max_depth = 0;
    for (bx_t const & arg : args) {
        auto depth = arg->depth();
        if (depth > max_depth) max_depth = depth;
    }
    return max_depth + 1;
}


uint32_t
Atom::size() const
{
    return 1;
}


uint32_t
Operator::size() const
{
    uint32_t size = 0;
    for (bx_t const & arg : args)
        size += arg->size();
    return size + 1;
}


uint32_t
Atom::atom_count() const
{
    return 1;
}


uint32_t
Operator::atom_count() const
{
    uint32_t atom_count = 0;
    for (bx_t const & arg : args)
        atom_count += arg->atom_count();
    return atom_count;
}


uint32_t
Atom::op_count() const
{
    return 0;
}


uint32_t
Operator::op_count() const
{
    uint32_t op_count = 0;
    for (bx_t const & arg : args)
        op_count += arg->op_count();
    return op_count + 1;
}
