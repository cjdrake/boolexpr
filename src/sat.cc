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
Zero::sat() const
{
    return std::make_pair(false, boost::none);
}


soln_t
One::sat() const
{
    return std::make_pair(true, point_t {});
}


soln_t
Logical::sat() const
{
    return std::make_pair(false, boost::none);
}


soln_t
Illogical::sat() const
{
    return std::make_pair(false, boost::none);
}


soln_t
Complement::sat() const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(~self);
    return std::make_pair(true, point_t { {x, zero()} });
}


soln_t
Variable::sat() const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    return std::make_pair(true, point_t { {x, one()} });
}


soln_t
Operator::sat() const
{
    auto self = shared_from_this();
    auto op = std::static_pointer_cast<Operator const>(self);

    auto ctx = Context();
    auto cnf = op->tseytin(ctx);

    auto s = cnf->support();
    unordered_map<bx_t, uint32_t> lit2idx;
    unordered_map<uint32_t, var_t> idx2var;

    uint32_t index = 0u;
    for (var_t const & x : s) {
        lit2idx.insert({~x, (index << 1) | 0u});
        lit2idx.insert({ x, (index << 1) | 1u});
        idx2var.insert({index, x});
        ++index;
    }

    SATSolver solver;
    vector<Lit> cmlits;
    solver.new_vars(s.size());

    auto and_op = std::static_pointer_cast<And const>(cnf);
    for (bx_t const & clause : and_op->args) {
        cmlits.clear();
        if (IS_LIT(clause)) {
            auto index = lit2idx.find(clause)->second;
            cmlits.push_back(Lit(index >> 1, !(index & 1u)));
        }
        else {
            auto or_op = std::static_pointer_cast<Or const>(clause);
            for (bx_t const & lit : or_op->args) {
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
