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


using CMSat::SATSolver;
using CMSat::Lit;
using CMSat::lbool;

using namespace boolexpr;


soln_t
Zero::_sat(const bx_t&) const
{
    return std::make_pair(false, boost::none);
}


soln_t
One::_sat(const bx_t&) const
{
    return std::make_pair(true, point_t {});
}


soln_t
Logical::_sat(const bx_t&) const
{
    return std::make_pair(false, boost::none);
}


soln_t
Illogical::_sat(const bx_t&) const
{
    return std::make_pair(false, boost::none);
}


soln_t
Complement::_sat(const bx_t& self) const
{
    auto x = std::static_pointer_cast<Variable>(~self);
    return std::make_pair(true, point_t { {x, zero()} });
}


soln_t
Variable::_sat(const bx_t& self) const
{
    auto x = std::static_pointer_cast<Variable>(self);
    return std::make_pair(true, point_t { {x, one()} });
}


soln_t
Operator::_sat(const bx_t& bx) const
{
    auto ctx = Context();
    auto cnf = tseytin(bx, ctx);

    auto s = support(cnf);
    unordered_map<bx_t, uint32_t> lit2idx;
    unordered_map<uint32_t, var_t> idx2var;

    uint32_t index = 0u;
    for (const var_t& x : s) {
        lit2idx.insert({~x, (index << 1) | 0u});
        lit2idx.insert({ x, (index << 1) | 1u});
        idx2var.insert({index, x});
        ++index;
    }

    SATSolver solver;
    vector<Lit> cmlits;
    solver.new_vars(s.size());

    auto and_op = std::static_pointer_cast<And>(cnf);
    for (const bx_t& clause : and_op->args) {
        cmlits.clear();
        if (IS_LIT(clause)) {
            auto index = lit2idx.find(clause)->second;
            cmlits.push_back(Lit(index >> 1, !(index & 1u)));
        }
        else {
            auto or_op = std::static_pointer_cast<Or>(clause);
            for (const bx_t& lit : or_op->args) {
                auto index = lit2idx.find(lit)->second;
                cmlits.push_back(Lit(index >> 1, !(index & 1u)));
            }
        }
        solver.add_clause(cmlits);
    }

    auto ret = solver.solve();

    if (ret == l_True) {
        auto model = solver.get_model();
        point_t point;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (model[i] == l_False)
                    point.insert({x, zero()});
                else if (model[i] == l_True)
                    point.insert({x, one()});
            }
        }
        return std::make_pair(true, std::move(point));
    }
    else {
        return std::make_pair(false, boost::none);
    }
}


soln_t
boolexpr::sat(const bx_t& self)
{
    return self->_sat(simplify(self));
}
