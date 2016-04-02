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

    auto xs = cnf->support();
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

    auto sat = solver.solve();

    if (sat == l_True) {
        auto model = solver.get_model();
        point_t soln;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (model[i] == l_False)
                    soln.insert({x, zero()});
                else if (model[i] == l_True)
                    soln.insert({x, one()});
            }
        }
        return std::make_pair(true, std::move(soln));
    }
    else {
        return std::make_pair(false, boost::none);
    }
}


sat_iter::sat_iter() : sat {l_False} {}


sat_iter::sat_iter(bx_t const & bx)
{
    _one_soln = false;

    if (IS_ZERO(bx) || IS_UNKNOWN(bx)) {
        sat = l_False;
        return;
    }

    if (IS_ONE(bx)) {
        sat = l_True;
        _one_soln = true;
        return;
    }

    if (IS_COMP(bx)) {
        sat = l_True;
        auto x = std::static_pointer_cast<Variable const>(~bx);
        soln.insert({x, zero()});
        _one_soln = true;
        return;
    }

    if (IS_VAR(bx)) {
        sat = l_True;
        auto x = std::static_pointer_cast<Variable const>(bx);
        soln.insert({x, one()});
        _one_soln = true;
        return;
    }

    // Operator

    auto op = std::static_pointer_cast<Operator const>(bx);
    auto cnf = op->tseytin(_ctx);

    auto xs = cnf->support();
    _solver.new_vars(xs.size());
    uint32_t index = 0u;
    for (var_t const & x : xs) {
        _lit2idx.insert({~x, (index << 1) | 0u});
        _lit2idx.insert({ x, (index << 1) | 1u});
        _idx2var.insert({index, x});
        ++index;
    }

    vector<CMSat::Lit> clause;
    auto and_op = std::static_pointer_cast<And const>(cnf);
    for (bx_t const & arg : and_op->args) {
        clause.clear();
        if (IS_LIT(arg)) {
            auto index = _lit2idx.find(arg)->second;
            clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
        }
        else {
            auto or_op = std::static_pointer_cast<Or const>(arg);
            for (bx_t const & lit : or_op->args) {
                auto index = _lit2idx.find(lit)->second;
                clause.push_back(CMSat::Lit(index >> 1, !(index & 1u)));
            }
        }
        _solver.add_clause(clause);
    }

    _get_soln();
}


void
sat_iter::_get_soln()
{
    soln.clear();

    sat = _solver.solve();
    if (sat == l_True) {
        auto model = _solver.get_model();
        vector<CMSat::Lit> clause;
        for (size_t i = 0; i < _solver.nVars(); ++i) {
            auto x = _idx2var.find(i)->second;
            if (x->ctx != &_ctx) {
                if (model[i] == l_False) {
                    soln.insert({x, zero()});
                    clause.push_back(CMSat::Lit(i, false));
                }
                else if (model[i] == l_True) {
                    soln.insert({x, one()});
                    clause.push_back(CMSat::Lit(i, true));
                }
            }
        }
        // Block this solution
        _solver.add_clause(clause);
    }
}


bool
sat_iter::operator==(sat_iter const & rhs) const
{
    return sat == rhs.sat;
}


bool
sat_iter::operator!=(sat_iter const & rhs) const
{
    return !(*this == rhs);
}


point_t const &
sat_iter::operator*() const
{
    return soln;
}


sat_iter const &
sat_iter::operator++()
{
    if (_one_soln) {
        sat = l_False;
        soln.clear();
    }
    else {
        _get_soln();
    }

    return *this;
}
