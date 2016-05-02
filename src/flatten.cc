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
    if (clauses.size() == 0)
        return std::move(clauses);

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
        return std::move(clauses);

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
Atom::to_cnf() const
{
    return shared_from_this();
}


bx_t
Nor::to_cnf() const
{
    return to_posop()->to_cnf();
}


bx_t
Or::to_cnf() const
{
    auto or_or_and = transform([](bx_t const & arg){return arg->to_dnf();});
    auto bx = or_or_and->simplify();

    if (IS_ATOM(bx))
        return bx;

    auto lop = std::static_pointer_cast<LatticeOperator const>(bx);

    if (lop->is_clause())
        return lop;

    auto clauses = _product(_absorb(_twolvl2clauses(lop)));

    vector<bx_t> args;
    for (auto const & clause : clauses)
        args.push_back(or_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return and_s(std::move(args));
}


bx_t
Nand::to_cnf() const
{
    return to_posop()->to_cnf();
}


bx_t
And::to_cnf() const
{
    auto and_and_or = transform([](bx_t const & arg){return arg->to_cnf();});
    auto bx = and_and_or->simplify();

    if (IS_ATOM(bx))
        return bx;

    auto lop = std::static_pointer_cast<LatticeOperator const>(bx);

    if (lop->is_clause())
        return lop;

    auto clauses = _absorb(_twolvl2clauses(lop));

    vector<bx_t> args;
    for (auto const & clause : clauses)
        args.push_back(or_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return and_s(std::move(args));
}


bx_t
Xnor::to_cnf() const
{
    return to_posop()->to_cnf();
}


bx_t
Xor::to_cnf() const
{
    size_t n = args.size();

    vector<bx_t> clauses;
    for (auto it = space_iter(n); it != space_iter(); ++it) {
        if (!it.parity()) {
            vector<bx_t> clause(n);
            for (size_t i = 0; i < n; ++i)
                clause[i] = (*it)[i] ? ~args[i] : args[i];
            clauses.push_back(or_(std::move(clause)));
        }
    }

    return and_(std::move(clauses))->to_cnf();
}


bx_t
Unequal::to_cnf() const
{
    size_t n = args.size();

    vector<bx_t> xs(n), xns(n);
    for (size_t i = 0; i < n; ++i) {
        xns[i] = ~args[i];
        xs[i] = args[i];
    }

    return (or_(std::move(xns)) & or_(std::move(xs)))->to_cnf();
}


bx_t
Equal::to_cnf() const
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1));

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] | args[j];
            terms[cnt++] = args[i] | ~args[j];
        }
    }

    return and_(std::move(terms))->to_cnf();
}


bx_t
NotImplies::to_cnf() const
{
    auto p = args[0];
    auto q = args[1];

    return (p & ~q)->to_cnf();
}


bx_t
Implies::to_cnf() const
{
    auto p = args[0];
    auto q = args[1];

    return (~p | q)->to_cnf();
}


bx_t
NotIfThenElse::to_cnf() const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return ((~s | ~d1) & (s | ~d0))->to_cnf();
}


bx_t
IfThenElse::to_cnf() const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return ((~s | d1) & (s | d0))->to_cnf();
}


bx_t
Atom::to_dnf() const
{ return shared_from_this(); }


bx_t
Nor::to_dnf() const
{
    return to_posop()->to_dnf();
}


bx_t
Or::to_dnf() const
{
    auto or_or_and = transform([](bx_t const & arg){return arg->to_dnf();});
    auto bx = or_or_and->simplify();

    if (IS_ATOM(bx))
        return bx;

    auto lop = std::static_pointer_cast<LatticeOperator const>(bx);

    if (lop->is_clause())
        return lop;

    auto clauses = _absorb(_twolvl2clauses(lop));

    vector<bx_t> args;
    for (auto const & clause : clauses)
        args.push_back(and_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return or_s(std::move(args));
}


bx_t
Nand::to_dnf() const
{
    return to_posop()->to_dnf();
}


bx_t
And::to_dnf() const
{
    auto and_and_or = transform([](bx_t const & arg){return arg->to_dnf();});
    auto bx = and_and_or->simplify();

    if (IS_ATOM(bx))
        return bx;

    auto lop = std::static_pointer_cast<LatticeOperator const>(bx);

    if (lop->is_clause())
        return lop;

    auto clauses = _product(_absorb(_twolvl2clauses(lop)));

    vector<bx_t> args;
    for (auto const & clause : clauses)
        args.push_back(and_s(vector<bx_t>(clause.cbegin(), clause.cend())));
    return or_s(std::move(args));
}


bx_t
Xnor::to_dnf() const
{
    return to_posop()->to_dnf();
}


bx_t
Xor::to_dnf() const
{
    size_t n = args.size();

    vector<bx_t> clauses;
    for (auto it = space_iter(n); it != space_iter(); ++it) {
        if (it.parity()) {
            vector<bx_t> clause(n);
            for (size_t i = 0; i < n; ++i)
                clause[i] = (*it)[i] ? args[i] : ~args[i];
            clauses.push_back(and_(std::move(clause)));
        }
    }

    return or_(std::move(clauses))->to_dnf();
}


bx_t
Unequal::to_dnf() const
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1));

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] & args[j];
            terms[cnt++] = args[i] & ~args[j];
        }
    }

    return or_(std::move(terms))->to_dnf();
}


bx_t
Equal::to_dnf() const
{
    size_t n = args.size();

    vector<bx_t> xs(n), xns(n);
    for (size_t i = 0; i < n; ++i) {
        xns[i] = ~args[i];
        xs[i] = args[i];
    }

    return (and_(std::move(xns)) | and_(std::move(xs)))->to_dnf();
}


bx_t
NotImplies::to_dnf() const
{
    auto p = args[0];
    auto q = args[1];

    return (p & ~q)->to_dnf();
}


bx_t
Implies::to_dnf() const
{
    auto p = args[0];
    auto q = args[1];

    return (~p | q)->to_dnf();
}


bx_t
NotIfThenElse::to_dnf() const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return ((s & ~d1) | (~s & ~d0))->to_dnf();
}


bx_t
IfThenElse::to_dnf() const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return ((s & d1) | (~s & d0))->to_dnf();
}
