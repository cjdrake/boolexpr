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


#include "boolexpr/boolexpr.h"


using std::make_pair;
using std::static_pointer_cast;
using std::unordered_map;
using std::vector;


// Required for l_False and l_True
using CMSat::lbool;


namespace boolexpr {


static void
encode_cmsat(std::unordered_map<uint32_t, var_t> & idx2var,
             CMSat::SATSolver & solver,
             bx_t bx)
{
    auto xs = bx->support();
    unordered_map<lit_t, uint32_t> lit2idx;

    uint32_t index = 0;
    for (var_t const & x : xs) {
        auto xn_lit = static_pointer_cast<Literal const>(~x);
        auto  x_lit = static_pointer_cast<Literal const>( x);
        lit2idx.insert({xn_lit, (index << 1) | 0u});
        lit2idx.insert({ x_lit, (index << 1) | 1u});
        idx2var.insert({index, x});
        ++index;
    }

    solver.new_vars(xs.size());

    if (IS_OR(bx)) {
        vector<CMSat::Lit> clause;
        auto or_op = static_pointer_cast<Or const>(bx);
        for (bx_t const & arg : or_op->args) {
            auto lit = static_pointer_cast<Literal const>(arg);
            auto index = lit2idx.find(lit)->second;
            clause.push_back(CMSat::Lit(index>>1, !(index&1u)));
        }
        solver.add_clause(std::move(clause));
    }
    else {
        auto and_op = static_pointer_cast<And const>(bx);
        for (bx_t const & arg : and_op->args) {
            vector<CMSat::Lit> clause;
            if (IS_OR(arg)) {
                auto or_op = static_pointer_cast<Or const>(arg);
                for (bx_t const & _arg : or_op->args) {
                    auto lit = static_pointer_cast<Literal const>(_arg);
                    auto index = lit2idx.find(lit)->second;
                    clause.push_back(CMSat::Lit(index>>1, !(index&1u)));
                }
            }
            else {
                auto lit = static_pointer_cast<Literal const>(arg);
                auto index = lit2idx.find(lit)->second;
                clause.push_back(CMSat::Lit(index>>1, !(index&1u)));
            }
            solver.add_clause(std::move(clause));
        }
    }
}


soln_t
BoolExpr::sat() const
{
    return simplify()->_sat();
}


soln_t
Zero::_sat() const
{
    return make_pair(false, boost::none);
}


soln_t
One::_sat() const
{
    return make_pair(true, point_t {});
}


soln_t
Logical::_sat() const
{
    return make_pair(false, boost::none);
}


soln_t
Illogical::_sat() const
{
    return make_pair(false, boost::none);
}


soln_t
Complement::_sat() const
{
    auto self = shared_from_this();
    auto x = static_pointer_cast<Variable const>(~self);
    return make_pair(true, point_t { {x, zero()} });
}


soln_t
Variable::_sat() const
{
    auto self = shared_from_this();
    auto x = static_pointer_cast<Variable const>(self);
    return make_pair(true, point_t { {x, one()} });
}


soln_t
Operator::_sat() const
{
    std::unordered_map<uint32_t, var_t> idx2var;
    CMSat::SATSolver solver;

    auto ctx = Context();
    auto cnf = tseytin(ctx);
    encode_cmsat(idx2var, solver, cnf);

    auto sat = solver.solve();

    if (sat == l_True) {
        auto model = solver.get_model();
        point_t point;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (model[i] == l_False) {
                    point.insert({x, zero()});
                }
                else if (model[i] == l_True) {
                    point.insert({x, one()});
                }
            }
        }
        return make_pair(true, std::move(point));
    }
    else {
        return make_pair(false, boost::none);
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
        auto x = static_pointer_cast<Variable const>(~bx);
        point.insert({x, zero()});
        one_soln = true;
        return;
    }

    if (IS_VAR(bx)) {
        sat = l_True;
        auto x = static_pointer_cast<Variable const>(bx);
        point.insert({x, one()});
        one_soln = true;
        return;
    }

    // Operator
    auto op = static_pointer_cast<Operator const>(bx);
    auto cnf = op->tseytin(ctx);
    encode_cmsat(idx2var, solver, cnf);

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
    return point;
}


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


}  // namespace boolexpr
