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

    return clauses;
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

    if (!drop) return clauses;

    vector<std::set<lit_t>> _clauses;
    for (size_t i = 0; i < clauses.size(); ++i)
        if (keep[i]) _clauses.push_back(clauses[i]);

    return _clauses;
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

    return product;
}


static bx_t _nnf2cnf(bx_t const &);
static bx_t _nnf2dnf(bx_t const &);


static bx_t
_nnf2cnf(bx_t const & nnf1)
{
    if (IS_ATOM(nnf1)) return nnf1;
    auto lop1 = std::static_pointer_cast<LatticeOperator const>(nnf1);
    if (lop1->is_clause()) return lop1;

    uint32_t mod_count = 0;
    vector<bx_t> _args;
    for (bx_t const & arg : lop1->args) {
        auto _arg = IS_OR(lop1) ? _nnf2dnf(arg) : _nnf2cnf(arg);
        mod_count += (_arg != arg);
        _args.push_back(_arg);
    }

    std::shared_ptr<LatticeOperator const> lop2;
    if (mod_count) {
        auto nnf2 = lop1->from_args(std::move(_args))->simplify();
        if (IS_ATOM(nnf2)) return nnf2;
        lop2 = std::move(std::static_pointer_cast<LatticeOperator const>(nnf2));
        if (lop2->is_clause()) return lop2;
    }
    else {
        lop2 = std::move(lop1);
    }

    auto clauses1 = _twolvl2clauses(lop2);
    auto clauses2 = _absorb(std::move(clauses1));
    auto clauses3 = IS_OR(lop2) ? _product(clauses2) : std::move(clauses2);

    vector<bx_t> args;
    for (auto const & clause : clauses3)
        args.push_back(or_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return and_s(std::move(args));
}


static bx_t
_nnf2dnf(bx_t const & nnf1)
{
    if (IS_ATOM(nnf1)) return nnf1;
    auto lop1 = std::static_pointer_cast<LatticeOperator const>(nnf1);
    if (lop1->is_clause()) return lop1;

    uint32_t mod_count = 0;
    vector<bx_t> _args;
    for (bx_t const & arg : lop1->args) {
        auto _arg = IS_OR(lop1) ? _nnf2dnf(arg) : _nnf2cnf(arg);
        mod_count += (_arg != arg);
        _args.push_back(_arg);
    }

    std::shared_ptr<LatticeOperator const> lop2;
    if (mod_count) {
        auto nnf2 = lop1->from_args(std::move(_args))->simplify();
        if (IS_ATOM(nnf2)) return nnf2;
        lop2 = std::move(std::static_pointer_cast<LatticeOperator const>(nnf2));
        if (lop2->is_clause()) return lop2;
    }
    else {
        lop2 = std::move(lop1);
    }

    auto clauses1 = _twolvl2clauses(lop2);
    auto clauses2 = _absorb(std::move(clauses1));
    auto clauses3 = IS_AND(lop2) ? _product(clauses2) : std::move(clauses2);

    vector<bx_t> args;
    for (auto const & clause : clauses3)
        args.push_back(and_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return or_s(std::move(args));
}


bx_t
BoolExpr::to_cnf() const
{
    return _nnf2cnf(to_nnf());
}


bx_t
BoolExpr::to_dnf() const
{
    return _nnf2dnf(to_nnf());
}
