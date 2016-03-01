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
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


BoolExpr::BoolExpr(Kind kind) : kind {kind} {}
Atom::Atom(Kind kind) : BoolExpr(kind) {}
Constant::Constant(Kind kind) : Atom(kind) {}
Known::Known(Kind kind, bool val) : Constant(kind), val {val} {}
Zero::Zero() : Known(ZERO, false) {}
One::One() : Known(ONE, true) {}
Unknown::Unknown(Kind kind) : Constant(kind) {}
Logical::Logical() : Unknown(LOG) {}
Illogical::Illogical() : Unknown(ILL) {}
Literal::Literal(Kind kind, Context *ctx, id_t id) : Atom(kind), ctx {ctx}, id {id} {}
Complement::Complement(Context *ctx, id_t id) : Literal(COMP, ctx, id) {}
Variable::Variable(Context *ctx, id_t id) : Literal(VAR, ctx, id) {}

Operator::Operator(Kind kind, bool simple, const vector<bx_t>& args)
    : BoolExpr(kind)
    , simple {simple}
    , args {args}
{}

Operator::Operator(Kind kind, bool simple, const vector<bx_t>&& args)
    : BoolExpr(kind)
    , simple {simple}
    , args {args}
{}

LatticeOperator::LatticeOperator(Kind kind, bool simple, const vector<bx_t>& args)
    : Operator(kind, simple, args)
{}

LatticeOperator::LatticeOperator(Kind kind, bool simple, const vector<bx_t>&& args)
    : Operator(kind, simple, args)
{}

Nor::Nor(bool simple, const vector<bx_t>& args) : Operator(NOR, simple, args) {}
Nor::Nor(bool simple, const vector<bx_t>&& args) : Operator(NOR, simple, args) {}
Or::Or(bool simple, const vector<bx_t>& args) : LatticeOperator(OR, simple, args) {}
Or::Or(bool simple, const vector<bx_t>&& args) : LatticeOperator(OR, simple, args) {}
Nand::Nand(bool simple, const vector<bx_t>& args) : Operator(NAND, simple, args) {}
Nand::Nand(bool simple, const vector<bx_t>&& args) : Operator(NAND, simple, args) {}
And::And(bool simple, const vector<bx_t>& args) : LatticeOperator(AND, simple, args) {}
And::And(bool simple, const vector<bx_t>&& args) : LatticeOperator(AND, simple, args) {}
Xnor::Xnor(bool simple, const vector<bx_t>& args) : Operator(XNOR, simple, args) {}
Xnor::Xnor(bool simple, const vector<bx_t>&& args) : Operator(XNOR, simple, args) {}
Xor::Xor(bool simple, const vector<bx_t>& args) : Operator(XOR, simple, args) {}
Xor::Xor(bool simple, const vector<bx_t>&& args) : Operator(XOR, simple, args) {}
Unequal::Unequal(bool simple, const vector<bx_t>& args) : Operator(NEQ, simple, args) {}
Unequal::Unequal(bool simple, const vector<bx_t>&& args) : Operator(NEQ, simple, args) {}

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


bx_t Or::identity() { return zero(); }
bx_t Or::dominator() { return one(); }

bx_t And::identity() { return one(); }
bx_t And::dominator() { return zero(); }

bx_t Xor::identity() { return zero(); }


dfs_iter
boolexpr::begin(const bx_t& bx)
{
    return dfs_iter(bx);
}

const dfs_iter
boolexpr::end(const bx_t&)
{
    return dfs_iter();
}


zero_t
boolexpr::zero()
{
    static auto _zero = std::make_shared<Zero>();
    return _zero;
}

one_t
boolexpr::one()
{
    static auto _one = std::make_shared<One>();
    return _one;
}

log_t
boolexpr::logical()
{
    static auto _log = std::make_shared<Logical>();
    return _log;
}

ill_t
boolexpr::illogical()
{
    static auto _ill = std::make_shared<Illogical>();
    return _ill;
}


bx_t
boolexpr::or_(const vector<bx_t>& args)
{
    if (args.size() == 0)
        return Or::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<Or>(false, args);
}

bx_t
boolexpr::or_(const vector<bx_t>&& args)
{
    if (args.size() == 0)
        return Or::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<Or>(false, args);
}

bx_t
boolexpr::or_(std::initializer_list<bx_t> l)
{
    return or_(vector<bx_t>(l.begin(), l.end()));
}

bx_t
boolexpr::and_(const vector<bx_t>& args)
{
    if (args.size() == 0)
        return And::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<And>(false, args);
}

bx_t
boolexpr::and_(const vector<bx_t>&& args)
{
    if (args.size() == 0)
        return And::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<And>(false, args);
}

bx_t
boolexpr::and_(std::initializer_list<bx_t> l)
{
    return and_(vector<bx_t>(l.begin(), l.end()));
}

bx_t
boolexpr::xor_(const vector<bx_t>& args)
{
    if (args.size() == 0)
        return Xor::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<Xor>(false, args);
}

bx_t
boolexpr::xor_(const vector<bx_t>&& args)
{
    if (args.size() == 0)
        return Xor::identity();
    else if (args.size() == 1)
        return *args.begin();
    else
        return std::make_shared<Xor>(false, args);
}

bx_t
boolexpr::xor_(std::initializer_list<bx_t> l)
{
    return xor_(vector<bx_t>(l.begin(), l.end()));
}

bx_t
boolexpr::eq(const vector<bx_t>& args)
{
    if (args.size() < 2)
        return one();
    else
        return std::make_shared<Equal>(false, args);
}

bx_t
boolexpr::eq(const vector<bx_t>&& args)
{
    if (args.size() < 2)
        return one();
    else
        return std::make_shared<Equal>(false, args);
}

bx_t
boolexpr::eq(std::initializer_list<bx_t> l)
{
    return eq(vector<bx_t>(l.begin(), l.end()));
}

bx_t
boolexpr::impl(const bx_t& p, const bx_t& q)
{
    return std::make_shared<Implies>(false, p, q);
}

bx_t
boolexpr::ite(const bx_t& s, const bx_t& d1, const bx_t& d0)
{
    return std::make_shared<IfThenElse>(false, s, d1, d0);
}


bx_t
boolexpr::or_s(const vector<bx_t>& args)
{ return simplify(or_(args)); }

bx_t
boolexpr::or_s(const vector<bx_t>&& args)
{ return simplify(or_(args)); }

bx_t
boolexpr::or_s(std::initializer_list<bx_t> l)
{ return simplify(or_(l)); }

bx_t
boolexpr::and_s(const vector<bx_t>& args)
{ return simplify(and_(args)); }

bx_t
boolexpr::and_s(const vector<bx_t>&& args)
{ return simplify(and_(args)); }

bx_t
boolexpr::and_s(std::initializer_list<bx_t> l)
{ return simplify(and_(l)); }

bx_t
boolexpr::xor_s(const vector<bx_t>& args)
{ return simplify(xor_(args)); }

bx_t
boolexpr::xor_s(const vector<bx_t>&& args)
{ return simplify(xor_(args)); }

bx_t
boolexpr::xor_s(std::initializer_list<bx_t> l)
{ return simplify(xor_(l)); }

bx_t
boolexpr::eq_s(const vector<bx_t>& args)
{ return simplify(eq(args)); }

bx_t
boolexpr::eq_s(const vector<bx_t>&& args)
{ return simplify(eq(args)); }

bx_t
boolexpr::eq_s(std::initializer_list<bx_t> l)
{ return simplify(eq(l)); }

bx_t
boolexpr::impl_s(const bx_t& p, const bx_t& q)
{ return simplify(impl(p, q)); }

bx_t
boolexpr::ite_s(const bx_t& s, const bx_t& d1, const bx_t& d0)
{ return simplify(ite(s, d1, d0)); }


// Overloaded Operators
bx_t boolexpr::operator~(const bx_t& self) { return self->invert(); }
bx_t boolexpr::operator|(const bx_t& lhs, const bx_t& rhs) { return or_({lhs, rhs}); }
bx_t boolexpr::operator&(const bx_t& lhs, const bx_t& rhs) { return and_({lhs, rhs}); }
bx_t boolexpr::operator^(const bx_t& lhs, const bx_t& rhs) { return xor_({lhs, rhs}); }

bool
boolexpr::operator<(const bx_t& lhs, const bx_t& rhs)
{
    if (IS_LIT(lhs) && IS_LIT(rhs)) {
        auto _lhs = std::static_pointer_cast<const Literal>(lhs);
        auto _rhs = std::static_pointer_cast<const Literal>(rhs);
        return _lhs->ctx == _rhs->ctx ? _lhs->id < _rhs->id
                                      : _lhs->ctx < _rhs->ctx;
    }
    else {
        return lhs->kind < rhs->kind;
    }
}

bool
boolexpr::operator<(const lit_t& lhs, const lit_t& rhs)
{
    return lhs->ctx == rhs->ctx ? lhs->id < rhs->id : lhs->ctx < rhs->ctx;
}


// String conversion
string Zero::_str(const bx_t&) const { return "0"; }
string One::_str(const bx_t&)  const { return "1"; }

string Logical::_str(const bx_t&)   const { return "X"; }
string Illogical::_str(const bx_t&) const { return "?"; }


string
Complement::_str(const bx_t& self) const
{
    auto xn = std::static_pointer_cast<const Complement>(self);
    return "~" + xn->ctx->get_name(xn->id);
}

string
Variable::_str(const bx_t& self) const
{
    auto x = std::static_pointer_cast<const Variable>(self);
    return x->ctx->get_name(x->id);
}


string
Operator::_tostr(const string opname) const
{
    std::stringstream ss;
    ss << opname << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0) ss << ", ";
        ss << str(args[i]);
    }
    ss << ")";
    return ss.str();
}

string Nor::_str(const bx_t& self)     const { return _tostr("Nor"); }
string Or::_str(const bx_t& self)      const { return _tostr("Or"); }
string Nand::_str(const bx_t& self)    const { return _tostr("Nand"); }
string And::_str(const bx_t& self)     const { return _tostr("And"); }
string Xnor::_str(const bx_t& self)    const { return _tostr("Xnor"); }
string Xor::_str(const bx_t& self)     const { return _tostr("Xor"); }
string Unequal::_str(const bx_t& self) const { return _tostr("Unequal"); }
string Equal::_str(const bx_t& self)   const { return _tostr("Equal"); }

string NotImplies::_str(const bx_t& self) const { return _tostr("NotImplies"); }
string Implies::_str(const bx_t& self)    const { return _tostr("Implies"); }

string NotIfThenElse::_str(const bx_t& self) const { return _tostr("NotIfThenElse"); }
string IfThenElse::_str(const bx_t& self)    const { return _tostr("IfThenElse"); }


op_t Nor::from_args(const vector<bx_t>& args) const { return std::make_shared<Nor>(false, args); }
op_t Nor::from_args(const vector<bx_t>&& args) const { return std::make_shared<Nor>(false, args); }
op_t Or::from_args(const vector<bx_t>& args) const { return std::make_shared<Or>(false, args); }
op_t Or::from_args(const vector<bx_t>&& args) const { return std::make_shared<Or>(false, args); }
op_t Nand::from_args(const vector<bx_t>& args) const { return std::make_shared<Nand>(false, args); }
op_t Nand::from_args(const vector<bx_t>&& args) const { return std::make_shared<Nand>(false, args); }
op_t And::from_args(const vector<bx_t>& args) const { return std::make_shared<And>(false, args); }
op_t And::from_args(const vector<bx_t>&& args) const { return std::make_shared<And>(false, args); }
op_t Xnor::from_args(const vector<bx_t>& args) const { return std::make_shared<Xnor>(false, args); }
op_t Xnor::from_args(const vector<bx_t>&& args) const { return std::make_shared<Xnor>(false, args); }
op_t Xor::from_args(const vector<bx_t>& args) const { return std::make_shared<Xor>(false, args); }
op_t Xor::from_args(const vector<bx_t>&& args) const { return std::make_shared<Xor>(false, args); }
op_t Unequal::from_args(const vector<bx_t>& args) const { return std::make_shared<Unequal>(false, args); }
op_t Unequal::from_args(const vector<bx_t>&& args) const { return std::make_shared<Unequal>(false, args); }
op_t Equal::from_args(const vector<bx_t>& args) const { return std::make_shared<Equal>(false, args); }
op_t Equal::from_args(const vector<bx_t>&& args) const { return std::make_shared<Equal>(false, args); }
op_t NotImplies::from_args(const vector<bx_t>& args) const { return std::make_shared<NotImplies>(false, args[0], args[1]); }
op_t NotImplies::from_args(const vector<bx_t>&& args) const { return std::make_shared<NotImplies>(false, args[0], args[1]); }
op_t Implies::from_args(const vector<bx_t>& args) const { return std::make_shared<Implies>(false, args[0], args[1]); }
op_t Implies::from_args(const vector<bx_t>&& args) const { return std::make_shared<Implies>(false, args[0], args[1]); }
op_t NotIfThenElse::from_args(const vector<bx_t>& args) const { return std::make_shared<NotIfThenElse>(false, args[0], args[1], args[2]); }
op_t NotIfThenElse::from_args(const vector<bx_t>&& args) const { return std::make_shared<NotIfThenElse>(false, args[0], args[1], args[2]); }
op_t IfThenElse::from_args(const vector<bx_t>& args) const { return std::make_shared<IfThenElse>(false, args[0], args[1], args[2]); }
op_t IfThenElse::from_args(const vector<bx_t>&& args) const { return std::make_shared<IfThenElse>(false, args[0], args[1], args[2]); }


string
boolexpr::str(const bx_t& self)
{
    return self->_str(self);
}


// Properties
bool
Operator::is_d1() const
{
    for (const bx_t& arg : args)
        if (IS_OP(arg)) return false;
    return true;
}


bool
Operator::is_clause() const
{
    for (const bx_t& arg : args)
        if (!IS_LIT(arg)) return false;
    return true;
}


bool Atom::is_dnf() const { return false; }
bool Zero::is_dnf() const { return true; }
bool Literal::is_dnf() const { return true; }
bool Operator::is_dnf() const { return false; }


bool
Or::is_dnf() const
{
    for (const bx_t& arg : args)
        if (!IS_LIT(arg) && !(IS_AND(arg) && std::static_pointer_cast<const And>(arg)->is_clause()))
            return false;
    return true;
}


bool Atom::is_cnf() const { return false; }
bool One::is_cnf() const { return true; }
bool Literal::is_cnf() const { return true; }
bool Operator::is_cnf() const { return false; }


bool
Or::is_cnf() const
{
    for (const bx_t& arg : args)
        if (!IS_LIT(arg))
            return false;
    return true;
}


bool
And::is_dnf() const
{
    for (const bx_t& arg : args)
        if (!IS_LIT(arg))
            return false;
    return true;
}


bool
And::is_cnf() const
{
    for (const bx_t& arg : args)
        if (!IS_LIT(arg) && !(IS_OR(arg) && std::static_pointer_cast<const Or>(arg)->is_clause()))
            return false;
    return true;
}


unordered_set<var_t>
boolexpr::support(const bx_t& self)
{
    unordered_set<var_t> s;

    for (auto it = dfs_iter(self); it != end(self); ++it) {
        if (IS_VAR(*it))
            s.insert(std::static_pointer_cast<const Variable>(*it));
        else if (IS_COMP(*it))
            s.insert(std::static_pointer_cast<const Variable>(~*it));
    }

    return s;
}


op_t
boolexpr::transform(const op_t& op, std::function<bx_t(const bx_t&)> f)
{
    uint32_t mod_count = 0;
    vector<bx_t> _args;

    for (const bx_t& arg : op->args) {
        auto _arg = f(arg);
        mod_count += (_arg != arg);
        _args.push_back(_arg);
    }

    if (mod_count)
        return op->from_args(std::move(_args));

    return op;
}


vector<bx_t>
boolexpr::cofactors(const bx_t& self, vector<var_t>& xs)
{
    vector<bx_t> cfs = {self};

    for (const var_t& x : xs) {
        vector<bx_t> temps;

        auto p0 = point_t { {x, zero()} };
        for (const bx_t& cf : cfs)
            temps.push_back(restrict_(cf, p0));

        auto p1 = point_t { {x, one()} };
        for (const bx_t& cf : cfs)
            temps.push_back(restrict_(cf, p1));

        std::swap(cfs, temps);
    }

    return cfs;
}


// FIXME(cjdrake): Rewrite this as a reduction
bx_t
boolexpr::smoothing(const bx_t& self, vector<var_t>& xs)
{
    return or_s(cofactors(self, xs));
}


// FIXME(cjdrake): Rewrite this as a reduction
bx_t
boolexpr::consensus(const bx_t& self, vector<var_t>& xs)
{
    return and_s(cofactors(self, xs));
}


// FIXME(cjdrake): Rewrite this as a reduction
bx_t
boolexpr::derivative(const bx_t& self, vector<var_t>& xs)
{
    return xor_s(cofactors(self, xs));
}
