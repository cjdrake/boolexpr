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


using Glucose::Lit;
using Glucose::lbool;  // l_False, l_True
using Glucose::mkLit;


namespace boolexpr {


static void
encode_cmsat(std::unordered_map<uint32_t, var_t> & idx2var,
             Glucose::Solver & solver,
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

    for (size_t i = 0; i < xs.size(); ++i)
        solver.newVar();

    if (IS_OR(bx)) {
        Glucose::vec<Lit> clause;
        auto or_op = static_pointer_cast<Or const>(bx);
        for (bx_t const & arg : or_op->args) {
            auto lit = static_pointer_cast<Literal const>(arg);
            auto index = lit2idx.find(lit)->second;
            clause.push(mkLit(index>>1, !(index&1u)));
        }
        solver.addClause(std::move(clause));
    }
    else {
        auto and_op = static_pointer_cast<And const>(bx);
        for (bx_t const & arg : and_op->args) {
            Glucose::vec<Lit> clause;
            if (IS_OR(arg)) {
                auto or_op = static_pointer_cast<Or const>(arg);
                for (bx_t const & _arg : or_op->args) {
                    auto lit = static_pointer_cast<Literal const>(_arg);
                    auto index = lit2idx.find(lit)->second;
                    clause.push(mkLit(index>>1, !(index&1u)));
                }
            }
            else {
                auto lit = static_pointer_cast<Literal const>(arg);
                auto index = lit2idx.find(lit)->second;
                clause.push(mkLit(index>>1, !(index&1u)));
            }
            solver.addClause(std::move(clause));
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
    Glucose::Solver solver;

    auto ctx = Context();
    auto cnf = tseytin(ctx);
    encode_cmsat(idx2var, solver, cnf);

    auto sat = solver.solve();

    if (sat) {
        point_t point;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (solver.modelValue(i) == l_False) {
                    point.insert({x, zero()});
                }
                else if (solver.modelValue(i) == l_True) {
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


void
Zero::sat_iter_init(sat_iter *it) const
{
    it->one_soln = false;
    it->sat = false;
}


void One::sat_iter_init(sat_iter *it) const
{
    it->one_soln = true;
    it->sat = true;
}


void Unknown::sat_iter_init(sat_iter *it) const
{
    it->one_soln = false;
    it->sat = false;
}


void Complement::sat_iter_init(sat_iter *it) const
{
    it->one_soln = true;
    it->sat = true;
    auto x = static_pointer_cast<Variable const>(~shared_from_this());
    it->point.insert({x, zero()});
}


void Variable::sat_iter_init(sat_iter *it) const
{
    it->one_soln = true;
    it->sat = true;
    auto x = static_pointer_cast<Variable const>(shared_from_this());
    it->point.insert({x, one()});
}


void Operator::sat_iter_init(sat_iter *it) const
{
    it->one_soln = false;
    auto cnf = tseytin(it->ctx);
    encode_cmsat(it->idx2var, it->solver, cnf);
    it->get_soln();
}


sat_iter::sat_iter()
    : sat {false}
{}


sat_iter::sat_iter(bx_t const & bx)
{
    bx->sat_iter_init(this);
}


void
sat_iter::get_soln()
{
    point.clear();

    sat = solver.solve();

    if (sat) {
        Glucose::vec<Lit> clause;
        for (size_t i = 0; i < solver.nVars(); ++i) {
            auto x = idx2var.find(i)->second;
            if (x->ctx != &ctx) {
                if (solver.modelValue(i) == l_False) {
                    point.insert({x, zero()});
                    clause.push(mkLit(i, false));
                }
                else if (solver.modelValue(i) == l_True) {
                    point.insert({x, one()});
                    clause.push(mkLit(i, true));
                }
            }
        }
        // Block this solution
        solver.addClause(clause);
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
        sat = false;
        point.clear();
    }
    else {
        get_soln();
    }

    return *this;
}


}  // namespace boolexpr
