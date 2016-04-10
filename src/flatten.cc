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
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>
#include <cryptominisat4/cryptominisat.h>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


static vector<std::set<lit_t>>
_twolvl2clauses(lop_t const & lop)
{
    vector<std::set<lit_t>> clauses;

    for (bx_t const & arg : lop->args) {
        std::set<lit_t> clause;
        if (IS_LIT(arg)) {
            clause.insert(std::static_pointer_cast<Literal const>(arg));
        }
        else {
            auto op = std::static_pointer_cast<Operator const>(arg);
            for (bx_t const & subarg : op->args)
                clause.insert(std::static_pointer_cast<Literal const>(subarg));
        }
        clauses.push_back(std::move(clause));
    }

    return std::move(clauses);
}


// Return a byte that shows set membership.
//
// xs <= ys: 1
// xs >= ys: 2
// xs == ys: 3

#define XS_LTE_YS (1u << 0)
#define YS_LTE_XS (1u << 1)

static uint8_t
_lits_cmp(std::set<lit_t> const & xs, std::set<lit_t> const & ys)
{
    uint8_t ret = XS_LTE_YS | YS_LTE_XS;

    auto xs_it = xs.begin();
    auto ys_it = ys.begin();

    while (xs_it != xs.end() && ys_it != ys.end()) {
        auto x = *xs_it;
        auto y = *ys_it;

        if (x == y) {
            ++xs_it;
            ++ys_it;
        }
        else {
            if (x->id < y->id) {
                ret &= ~XS_LTE_YS;
                ++xs_it;
            }
            else if (x->id > y->id) {
                ret &= ~YS_LTE_XS;
                ++ys_it;
            }
        }
    }

    if (xs_it != xs.end())
        ret &= ~XS_LTE_YS;

    if (ys_it != ys.end())
        ret &= ~YS_LTE_XS;

    return ret;
}


static vector<std::set<lit_t>>
_absorb(vector<std::set<lit_t>> const && clauses)
{
    vector<bool> keep;
    for (auto const & clause : clauses)
        keep.push_back(true);

    bool drop = false;
    for (size_t i = 0; i < (clauses.size() - 1); ++i) {
        if (keep[i]) {
            for (size_t j = i + 1; j < clauses.size(); ++j) {
                auto cmp = _lits_cmp(clauses[i], clauses[j]);
                // xs <= ys
                if (cmp & XS_LTE_YS) {
                    keep[j] = false;
                    drop = true;
                }
                // xs > ys
                else if (cmp & YS_LTE_XS) {
                    keep[i] = false;
                    drop = true;
                    break;
                }
            }
        }
    }

    if (!drop)
        return clauses;

    vector<std::set<lit_t>> kept_clauses;
    for (size_t i = 0; i < clauses.size(); ++i)
        if (keep[i]) kept_clauses.push_back(clauses[i]);

    return std::move(kept_clauses);
}


// NOTE: Return size is MxN
static vector<std::set<lit_t>>
_product(vector<std::set<lit_t>> const & clauses)
{
    vector<std::set<lit_t>> product {{}};

    for (auto const & clause : clauses) {
        vector<std::set<lit_t>> newprod;
        for (auto const & factor : product) {
            for (lit_t const & x : clause) {
                auto xn = std::static_pointer_cast<Literal const>(~x);
                if (factor.find(xn) == factor.end()) {
                    newprod.push_back(factor);
                    newprod.back().insert(x);
                }
            }
        }
        product = _absorb(std::move(newprod));
    }

    return std::move(product);
}


bx_t
Atom::nnf2cnf1() const
{ return shared_from_this(); }


bx_t
Operator::nnf2cnf1() const
{
    auto self = shared_from_this();

    if (is_clause())
        return self;

    auto lop = std::static_pointer_cast<LatticeOperator const>(self);

    auto f = IS_OR(lop) ? [](bx_t const & arg){return arg->nnf2dnf1();}
                        : [](bx_t const & arg){return arg->nnf2cnf1();} ;

    return lop->transform(f)->simplify()->nnf2cnf2();
}


bx_t
Atom::nnf2cnf2() const
{ return shared_from_this(); }


bx_t
Operator::nnf2cnf2() const
{
    auto self = shared_from_this();

    if (is_clause())
        return self;

    auto lop = std::static_pointer_cast<LatticeOperator const>(self);

    auto clauses1 = _twolvl2clauses(lop);
    auto clauses2 = _absorb(std::move(clauses1));
    auto clauses3 = IS_OR(lop) ? _product(clauses2) : std::move(clauses2);

    vector<bx_t> args;
    for (auto const & clause : clauses3)
        args.push_back(or_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return and_s(std::move(args));
}


bx_t
Atom::nnf2dnf1() const
{ return shared_from_this(); }


bx_t
Operator::nnf2dnf1() const
{
    auto self = shared_from_this();

    if (is_clause())
        return self;

    auto lop = std::static_pointer_cast<LatticeOperator const>(self);

    auto f = IS_OR(lop) ? [](bx_t const & arg){return arg->nnf2dnf1();}
                        : [](bx_t const & arg){return arg->nnf2cnf1();} ;

    return lop->transform(f)->simplify()->nnf2dnf2();
}


bx_t
Atom::nnf2dnf2() const
{ return shared_from_this(); }


bx_t
Operator::nnf2dnf2() const
{
    auto self = shared_from_this();

    if (is_clause())
        return self;

    auto lop = std::static_pointer_cast<LatticeOperator const>(self);

    auto clauses1 = _twolvl2clauses(lop);
    auto clauses2 = _absorb(std::move(clauses1));
    auto clauses3 = IS_AND(lop) ? _product(clauses2) : std::move(clauses2);

    vector<bx_t> args;
    for (auto const & clause : clauses3)
        args.push_back(and_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return or_s(std::move(args));
}


bx_t
BoolExpr::to_cnf() const
{ return to_nnf()->nnf2cnf1(); }


bx_t
BoolExpr::to_dnf() const
{ return to_nnf()->nnf2dnf1(); }
