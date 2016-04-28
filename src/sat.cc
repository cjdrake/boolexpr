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


// Required for l_False and l_True
using CMSat::lbool;

using namespace boolexpr;


soln_t
BoolExpr::sat() const
{ return simplify()->_sat(); }


soln_t
Zero::_sat() const
{ return std::make_pair(false, boost::none); }


soln_t
One::_sat() const
{ return std::make_pair(true, point_t {}); }


soln_t
Logical::_sat() const
{ return std::make_pair(false, boost::none); }


soln_t
Illogical::_sat() const
{ return std::make_pair(false, boost::none); }


soln_t
Complement::_sat() const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(~self);
    return std::make_pair(true, point_t { {x, zero()} });
}


soln_t
Variable::_sat() const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    return std::make_pair(true, point_t { {x, one()} });
}


soln_t
Operator::_sat() const
{
    auto ctx = Context();
    auto bx = tseytin(ctx);

    auto xs = bx->support();
    std::unordered_map<bx_t, uint32_t> lit2idx;
    std::unordered_map<uint32_t, var_t> idx2var;

    uint32_t index = 0u;
    for (var_t const & x : xs) {
        lit2idx.insert({~x, (index << 1) | 0u});
        lit2idx.insert({ x, (index << 1) | 1u});
        idx2var.insert({index, x});
        ++index;
    }

    CMSat::SATSolver solver;
    vector<CMSat::Lit> clause;
    solver.new_vars(xs.size());

    auto and_op = std::static_pointer_cast<And const>(bx);
    for (bx_t const & arg : and_op->args) {
        clause.clear();
        if (IS_LIT(arg)) {
            auto index = lit2idx.find(arg)->second;
            clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
        }
        else {
            auto or_op = std::static_pointer_cast<Or const>(arg);
            for (bx_t const & lit : or_op->args) {
                auto index = lit2idx.find(lit)->second;
                clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
            }
        }
        solver.add_clause(clause);
    }

    auto sat = solver.solve();

    if (sat == l_True) {
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


sat_iter::sat_iter()
    : sat {l_False}
{}


sat_iter::sat_iter(bx_t const & bx)
{
    one_soln = false;

    if (IS_ZERO(bx) || IS_UNKNOWN(bx)) {
        sat = l_False;
        return;
    }

    if (IS_ONE(bx)) {
        sat = l_True;
        one_soln = true;
        return;
    }

    if (IS_COMP(bx)) {
        sat = l_True;
        auto x = std::static_pointer_cast<Variable const>(~bx);
        point.insert({x, zero()});
        one_soln = true;
        return;
    }

    if (IS_VAR(bx)) {
        sat = l_True;
        auto x = std::static_pointer_cast<Variable const>(bx);
        point.insert({x, one()});
        one_soln = true;
        return;
    }

    // Operator

    auto op = std::static_pointer_cast<Operator const>(bx);
    auto cnf = op->tseytin(ctx);

    auto xs = cnf->support();
    solver.new_vars(xs.size());
    uint32_t index = 0u;
    for (var_t const & x : xs) {
        lit2idx.insert({~x, (index << 1) | 0u});
        lit2idx.insert({ x, (index << 1) | 1u});
        idx2var.insert({index, x});
        ++index;
    }

    vector<CMSat::Lit> clause;

    auto and_op = std::static_pointer_cast<And const>(cnf);
    for (bx_t const & arg : and_op->args) {
        clause.clear();
        if (IS_LIT(arg)) {
            auto index = lit2idx.find(arg)->second;
            clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
        }
        else {
            auto or_op = std::static_pointer_cast<Or const>(arg);
            for (bx_t const & lit : or_op->args) {
                auto index = lit2idx.find(lit)->second;
                clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
            }
        }
        solver.add_clause(clause);
    }

    get_soln();
}


void
sat_iter::get_soln()
{
    point.clear();

    sat = solver.solve();

    if (sat == l_True) {
        auto model = solver.get_model();
        vector<CMSat::Lit> clause;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (model[i] == l_False) {
                    point.insert({x, zero()});
                    clause.push_back(CMSat::Lit(i, false));
                }
                else if (model[i] == l_True) {
                    point.insert({x, one()});
                    clause.push_back(CMSat::Lit(i, true));
                }
            }
        }
        // Block this solution
        solver.add_clause(clause);
    }
}


bool
sat_iter::operator==(sat_iter const & rhs) const
{ return sat == rhs.sat; }


bool
sat_iter::operator!=(sat_iter const & rhs) const
{ return !(*this == rhs); }


point_t const &
sat_iter::operator*() const
{ return point; }


sat_iter const &
sat_iter::operator++()
{
    if (one_soln) {
        sat = l_False;
        point.clear();
    }
    else {
        get_soln();
    }

    return *this;
}
