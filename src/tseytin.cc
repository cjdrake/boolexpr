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


var_t
Operator::to_con1(Context& ctx, string const & auxvarname,
                  uint32_t& index, var2op_t& constraints) const
{
    auto key = ctx.get_var(auxvarname + "_" + std::to_string(index++));
    auto val = to_con2(ctx, auxvarname, index, constraints);

    constraints.insert({key, val});

    return key;
}


op_t
Operator::to_con2(Context& ctx, string const & auxvarname,
                  uint32_t& index, var2op_t& constraints) const
{
    bool found_subop = false;

    size_t n = args.size();
    vector<bx_t> _args(n);

    // NOTE: do not use transform, b/c there's mutable state
    for (size_t i = 0; i < n; ++i) {
        if (IS_OP(args[i])) {
            found_subop = true;
            auto subop = std::static_pointer_cast<Operator const>(args[i]);
            _args[i] = subop->to_con1(ctx, auxvarname, index, constraints);
        }
        else {
            _args[i] = args[i];
        }
    }

    if (found_subop)
        return from_args(std::move(_args));

    return std::static_pointer_cast<Operator const>(shared_from_this());
}


bx_t
Atom::tseytin(Context&, string const &) const
{
    return shared_from_this();
}


bx_t
Operator::tseytin(Context& ctx, string const & auxvarname) const
{
    uint32_t index {0};
    var2op_t constraints;

    auto top = to_con1(ctx, auxvarname, index, constraints);

    vector<bx_t> cnfs {top};
    for (auto const & constraint : constraints)
        cnfs.push_back(constraint.second->eqvar(constraint.first));

    return and_s(std::move(cnfs));
}


bx_t
Nor::eqvar(var_t const & x) const
{
    // x = ~(a | b | ...) <=> (~x | ~a) & (~x | ~b) & ... & (x | a | b | ...)

    vector<bx_t> clauses;

    for (bx_t const & arg : args)
        clauses.push_back(~x | ~arg);

    vector<bx_t> lits {x};
    for (bx_t const & arg : args)
        lits.push_back(arg);

    clauses.push_back(or_(std::move(lits)));

    return and_s(std::move(clauses));
}


bx_t
Or::eqvar(var_t const & x) const
{
    // x = a | b | ... <=> (x | ~a) & (x | ~b) & ... & (~x | a | b | ...)

    vector<bx_t> clauses;

    for (bx_t const & arg : args)
        clauses.push_back(x | ~arg);

    vector<bx_t> lits {~x};
    for (bx_t const & arg : args)
        lits.push_back(arg);

    clauses.push_back(or_(std::move(lits)));

    return and_s(std::move(clauses));
}


bx_t
Nand::eqvar(var_t const & x) const
{
    // x = ~(a & b & ...) <=> (x | a) & (x | b) & ... & (~x | ~a | ~b | ...)

    vector<bx_t> clauses;

    for (bx_t const & arg : args)
        clauses.push_back(x | arg);

    vector<bx_t> lits {~x};
    for (bx_t const & arg : args)
        lits.push_back(~arg);

    clauses.push_back(or_(std::move(lits)));

    return and_s(std::move(clauses));
}


bx_t
And::eqvar(var_t const & x) const
{
    // x = a & b & ... <=> (~x | a) & (~x | b) & ... & (x | ~a | ~b | ...)

    vector<bx_t> clauses;

    for (bx_t const & arg : args)
        clauses.push_back(~x | arg);

    vector<bx_t> lits {x};
    for (bx_t const & arg : args)
        lits.push_back(~arg);

    clauses.push_back(or_(std::move(lits)));

    return and_s(std::move(clauses));
}


bx_t
Xnor::eqvar(var_t const & x) const
{
    vector<vector<bx_t>> stack { vector<bx_t> {x} };

    for (bx_t const & arg : args) {
        vector<vector<bx_t>> temp;

        while (stack.size() > 0) {
            auto lits = stack.back();

            vector<bx_t> fst {lits[0]};
            vector<bx_t> snd {~lits[0]};
            for (auto it = lits.cbegin() + 1; it != lits.cend(); ++it) {
                fst.push_back(*it);
                snd.push_back(*it);
            }
            fst.push_back(arg);
            snd.push_back(~arg);

            temp.push_back(std::move(fst));
            temp.push_back(std::move(snd));

            stack.pop_back();
        }

        stack = std::move(temp);
    }

    vector<bx_t> clauses;
    while (stack.size() > 0) {
        auto lits = stack.back();
        stack.pop_back();
        clauses.push_back(or_(std::move(lits)));
    }

    return and_s(std::move(clauses));
}


bx_t
Xor::eqvar(var_t const & x) const
{
    vector<vector<bx_t>> stack { vector<bx_t> {~x} };

    for (bx_t const & arg : args) {
        vector<vector<bx_t>> temp;

        while (stack.size() > 0) {
            auto lits = stack.back();

            vector<bx_t> fst {lits[0]};
            vector<bx_t> snd {~lits[0]};
            for (auto it = lits.cbegin() + 1; it != lits.cend(); ++it) {
                fst.push_back(*it);
                snd.push_back(*it);
            }
            fst.push_back(arg);
            snd.push_back(~arg);

            temp.push_back(std::move(fst));
            temp.push_back(std::move(snd));

            stack.pop_back();
        }

        stack = std::move(temp);
    }

    vector<bx_t> clauses;
    while (stack.size() > 0) {
        auto lits = stack.back();
        stack.pop_back();
        clauses.push_back(or_(std::move(lits)));
    }

    return and_s(std::move(clauses));
}


bx_t
Unequal::eqvar(var_t const & x) const
{
    vector<bx_t> clauses;

    vector<bx_t> lits1 {~x};
    for (bx_t const & arg : args)
        lits1.push_back(arg);
    clauses.push_back(or_(std::move(lits1)));

    vector<bx_t> lits2 {~x};
    for (bx_t const & arg : args)
        lits2.push_back(~arg);
    clauses.push_back(or_(std::move(lits2)));

    for (auto i = 0u; i < args.size(); ++i) {
        for (auto j = i + 1u; j < args.size(); ++j) {
            clauses.push_back(x | ~args[i] | args[j]);
            clauses.push_back(x | args[i] | ~args[j]);
        }
    }

    return and_s(std::move(clauses));
}


bx_t
Equal::eqvar(var_t const & x) const
{
    vector<bx_t> clauses;

    vector<bx_t> lits1 {x};
    for (bx_t const & arg : args)
        lits1.push_back(arg);
    clauses.push_back(or_(std::move(lits1)));

    vector<bx_t> lits2 {x};
    for (bx_t const & arg : args)
        lits2.push_back(~arg);
    clauses.push_back(or_(std::move(lits2)));

    for (auto i = 0u; i < args.size(); ++i) {
        for (auto j = i + 1u; j < args.size(); ++j) {
            clauses.push_back(~x | ~args[i] | args[j]);
            clauses.push_back(~x | args[i] | ~args[j]);
        }
    }

    return and_s(std::move(clauses));
}


bx_t
NotImplies::eqvar(var_t const & x) const
{
    auto p = args[0];
    auto q = args[1];

    return and_s({(~x | p), (~x | ~q), (x | ~p | q)});
}


bx_t
Implies::eqvar(var_t const & x) const
{
    auto p = args[0];
    auto q = args[1];

    return and_s({(x | p), (x | ~q), (~x | ~p | q)});
}


bx_t
NotIfThenElse::eqvar(var_t const & x) const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return and_s({(~x | ~s | ~d1), (~x | s | ~d0), (x | ~s | d1), (x | s | d0), (x | d1 | d0)});
}


bx_t
IfThenElse::eqvar(var_t const & x) const
{
    auto s = args[0];
    auto d1 = args[1];
    auto d0 = args[2];

    return and_s({(x | ~s | ~d1), (x | s | ~d0), (~x | ~s | d1), (~x | s | d0), (~x | d1 | d0)});
}
