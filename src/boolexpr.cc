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


using std::make_shared;
using std::static_pointer_cast;
using std::unordered_set;
using std::vector;


namespace boolexpr {


BoolExpr::BoolExpr(Kind kind)
    : kind {kind}
{}


Atom::Atom(Kind kind)
    : BoolExpr(kind)
{}


Constant::Constant(Kind kind)
    : Atom(kind)
{}


Known::Known(Kind kind, bool val)
    : Constant(kind)
    , val {val}
{}


Zero::Zero()
    : Known(ZERO, false)
{}


One::One()
    : Known(ONE, true)
{}


Unknown::Unknown(Kind kind)
    : Constant(kind)
{}


Logical::Logical()
    : Unknown(LOG)
{}


Illogical::Illogical()
    : Unknown(ILL)
{}


Literal::Literal(Kind kind, Context * const ctx, id_t id)
    : Atom(kind)
    , ctx {ctx}
    , id {id}
{}


Complement::Complement(Context * const ctx, id_t id)
    : Literal(COMP, ctx, id)
{}


Variable::Variable(Context * const ctx, id_t id)
    : Literal(VAR, ctx, id)
{}


Operator::Operator(Kind kind, bool simple, vector<bx_t> const & args)
    : BoolExpr(kind)
    , simple {simple}
    , args {args}
{}


Operator::Operator(Kind kind, bool simple, vector<bx_t> const && args)
    : BoolExpr(kind)
    , simple {simple}
    , args {args}
{}


LatticeOperator::LatticeOperator(Kind kind, bool simple,
                                 vector<bx_t> const & args)
    : Operator(kind, simple, args)
{}


Nor::Nor(bool simple, vector<bx_t> const & args)
    : Operator(NOR, simple, args)
{}


Or::Or(bool simple, vector<bx_t> const & args)
    : LatticeOperator(OR, simple, args)
{}


Or::Or(bool simple, vector<bx_t> const && args)
    : LatticeOperator(OR, simple, args)
{}


Nand::Nand(bool simple, vector<bx_t> const & args)
    : Operator(NAND, simple, args)
{}


And::And(bool simple, vector<bx_t> const & args)
    : LatticeOperator(AND, simple, args)
{}


And::And(bool simple, vector<bx_t> const && args)
    : LatticeOperator(AND, simple, args)
{}


Xnor::Xnor(bool simple, vector<bx_t> const & args)
    : Operator(XNOR, simple, args)
{}


Xor::Xor(bool simple, vector<bx_t> const & args)
    : Operator(XOR, simple, args)
{}


Xor::Xor(bool simple, vector<bx_t> const && args)
    : Operator(XOR, simple, args)
{}


Unequal::Unequal(bool simple, vector<bx_t> const & args)
    : Operator(NEQ, simple, args)
{}


NotImplies::NotImplies(bool simple, bx_t p, bx_t q)
    : Operator(NIMPL, simple, vector<bx_t>{p, q})
{}


Implies::Implies(bool simple, bx_t p, bx_t q)
    : Operator(IMPL, simple, vector<bx_t>{p, q})
{}


NotIfThenElse::NotIfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0)
    : Operator(NITE, simple, vector<bx_t>{s, d1, d0})
{}


IfThenElse::IfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0)
    : Operator(ITE, simple, vector<bx_t>{s, d1, d0})
{}


bx_t
Or::identity()
{
    return zero();
}


bx_t
Or::dominator()
{
    return one();
}


bx_t
And::identity()
{
    return one();
}


bx_t
And::dominator()
{
    return zero();
}


bx_t
Xor::identity()
{
    return zero();
}


op_t
Nor::from_args(vector<bx_t> const && args) const
{
    return make_shared<Nor>(false, args);
}


op_t
Or::from_args(vector<bx_t> const && args) const
{
    return make_shared<Or>(false, args);
}


op_t
Nand::from_args(vector<bx_t> const && args) const
{
    return make_shared<Nand>(false, args);
}


op_t
And::from_args(vector<bx_t> const && args) const
{
    return make_shared<And>(false, args);
}


op_t
Xnor::from_args(vector<bx_t> const && args) const
{
    return make_shared<Xnor>(false, args);
}


op_t
Xor::from_args(vector<bx_t> const && args) const
{
    return make_shared<Xor>(false, args);
}


op_t
Unequal::from_args(vector<bx_t> const && args) const
{
    return make_shared<Unequal>(false, args);
}


op_t
Equal::from_args(vector<bx_t> const && args) const
{
    return make_shared<Equal>(false, args);
}


op_t
NotImplies::from_args(vector<bx_t> const && args) const
{
    return make_shared<NotImplies>(false, args[0], args[1]);
}


op_t
Implies::from_args(vector<bx_t> const && args) const
{
    return make_shared<Implies>(false, args[0], args[1]);
}


op_t
NotIfThenElse::from_args(vector<bx_t> const && args) const
{
    return make_shared<NotIfThenElse>(false, args[0], args[1], args[2]);
}


op_t
IfThenElse::from_args(vector<bx_t> const && args) const
{
    return make_shared<IfThenElse>(false, args[0], args[1], args[2]);
}


// Properties
bool
Operator::is_clause() const
{
    for (bx_t const & arg : args) {
        if (!IS_LIT(arg)) {
            return false;
        }
    }
    return true;
}


bool
Atom::is_cnf() const
{
    return false;
}


bool
One::is_cnf() const
{
    return true;
}


bool
Literal::is_cnf() const
{
    return true;
}


bool
Operator::is_cnf() const
{
    return false;
}


bool
Or::is_cnf() const
{
    for (bx_t const & arg : args) {
        if (!IS_LIT(arg)) {
            return false;
        }
    }
    return true;
}


bool
And::is_cnf() const
{
    for (bx_t const & arg : args) {
        if (!IS_LIT(arg) && !(IS_OR(arg) && static_pointer_cast<Or const>(arg)->is_clause())) {
            return false;
        }
    }
    return true;
}


bool
Atom::is_dnf() const
{
    return false;
}


bool
Zero::is_dnf() const
{
    return true;
}


bool
Literal::is_dnf() const
{
    return true;
}


bool
Operator::is_dnf() const
{
    return false;
}


bool
Or::is_dnf() const
{
    for (bx_t const & arg : args) {
        if (!IS_LIT(arg) && !(IS_AND(arg) && static_pointer_cast<And const>(arg)->is_clause())) {
            return false;
        }
    }
    return true;
}


bool
And::is_dnf() const
{
    for (bx_t const & arg : args) {
        if (!IS_LIT(arg)) {
            return false;
        }
    }
    return true;
}


unordered_set<var_t>
BoolExpr::support() const
{
    auto self = shared_from_this();

    unordered_set<var_t> s;

    for (auto it = dfs_iter(self); it != dfs_iter(); ++it) {
        if (IS_VAR(*it)) {
            s.insert(static_pointer_cast<Variable const>(*it));
        }
        else if (IS_COMP(*it)) {
            s.insert(static_pointer_cast<Variable const>(~*it));
        }
    }

    return std::move(s);
}


uint32_t
BoolExpr::degree() const
{
    return support().size();
}


op_t
Operator::transform(std::function<bx_t(bx_t const &)> f) const
{
    uint32_t mod_count = 0;
    size_t n = args.size();
    vector<bx_t> _args(n);

    for (size_t i = 0; i < n; ++i) {
        auto arg = args[i];
        auto _arg = f(arg);
        mod_count += (_arg != arg);
        _args[i] = _arg;
    }

    if (mod_count) {
        return from_args(std::move(_args));
    }

    return static_pointer_cast<Operator const>(shared_from_this());
}


bx_t
BoolExpr::expand(vector<var_t> const & xs) const
{
    auto self = shared_from_this();

    vector<bx_t> or_args;

    auto it1 = terms_iter(xs);
    auto it2 = cf_iter(self, xs);

    for (; it1 != terms_iter() && it2 != cf_iter(); ++it1, ++it2) {
        vector<bx_t> and_args;
        for (auto const & term : *it1) {
            and_args.push_back(term);
        }
        and_args.push_back(*it2);
        or_args.push_back(and_(std::move(and_args)));
    }

    return or_(std::move(or_args));
}


// FIXME(cjdrake): Implement these as reductions
bx_t
BoolExpr::smoothing(vector<var_t> const & xs) const
{
    auto self = shared_from_this();
    return or_s(vector<bx_t>(cf_iter(self, xs), cf_iter()));
}


bx_t
BoolExpr::consensus(vector<var_t> const & xs) const
{
    auto self = shared_from_this();
    return and_s(vector<bx_t>(cf_iter(self, xs), cf_iter()));
}


bx_t
BoolExpr::derivative(vector<var_t> const & xs) const
{
    auto self = shared_from_this();
    return xor_s(vector<bx_t>(cf_iter(self, xs), cf_iter()));
}


}  // namespace boolexpr
