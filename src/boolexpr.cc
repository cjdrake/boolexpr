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
#include <cryptominisat4/cryptominisat.h>

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
Literal::Literal(Kind kind, Context * const ctx, id_t id) : Atom(kind), ctx {ctx}, id {id} {}
Complement::Complement(Context * const ctx, id_t id) : Literal(COMP, ctx, id) {}
Variable::Variable(Context * const ctx, id_t id) : Literal(VAR, ctx, id) {}

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

LatticeOperator::LatticeOperator(Kind kind, bool simple, vector<bx_t> const & args)
    : Operator(kind, simple, args)
{}

Nor::Nor(bool simple, vector<bx_t> const & args) : Operator(NOR, simple, args) {}
Or::Or(bool simple, vector<bx_t> const & args) : LatticeOperator(OR, simple, args) {}
Or::Or(bool simple, vector<bx_t> const && args) : LatticeOperator(OR, simple, args) {}
Nand::Nand(bool simple, vector<bx_t> const & args) : Operator(NAND, simple, args) {}
And::And(bool simple, vector<bx_t> const & args) : LatticeOperator(AND, simple, args) {}
And::And(bool simple, vector<bx_t> const && args) : LatticeOperator(AND, simple, args) {}
Xnor::Xnor(bool simple, vector<bx_t> const & args) : Operator(XNOR, simple, args) {}
Xor::Xor(bool simple, vector<bx_t> const & args) : Operator(XOR, simple, args) {}
Xor::Xor(bool simple, vector<bx_t> const && args) : Operator(XOR, simple, args) {}
Unequal::Unequal(bool simple, vector<bx_t> const & args) : Operator(NEQ, simple, args) {}

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


bx_t boolexpr::nor(vector<bx_t> const & args) { return ~or_(args); }
bx_t boolexpr::nor(vector<bx_t> const && args) { return ~or_(args); }
bx_t boolexpr::nor(std::initializer_list<bx_t> const args) { return ~or_(args); }


bx_t
boolexpr::or_(vector<bx_t> const & args)
{
    if (args.size() == 0)
        return Or::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<Or>(false, args);
}

bx_t
boolexpr::or_(vector<bx_t> const && args)
{
    if (args.size() == 0)
        return Or::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<Or>(false, args);
}

bx_t
boolexpr::or_(std::initializer_list<bx_t> const args)
{
    return or_(vector<bx_t>(args.begin(), args.end()));
}


bx_t boolexpr::nand(vector<bx_t> const & args) { return ~and_(args); }
bx_t boolexpr::nand(vector<bx_t> const && args) { return ~and_(args); }
bx_t boolexpr::nand(std::initializer_list<bx_t> const args) { return ~and_(args); }


bx_t
boolexpr::and_(vector<bx_t> const & args)
{
    if (args.size() == 0)
        return And::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<And>(false, args);
}

bx_t
boolexpr::and_(vector<bx_t> const && args)
{
    if (args.size() == 0)
        return And::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<And>(false, args);
}

bx_t
boolexpr::and_(std::initializer_list<bx_t> const args)
{
    return and_(vector<bx_t>(args.begin(), args.end()));
}


bx_t boolexpr::xnor(vector<bx_t> const & args) { return ~xor_(args); }
bx_t boolexpr::xnor(vector<bx_t> const && args) { return ~xor_(args); }
bx_t boolexpr::xnor(std::initializer_list<bx_t> const args) { return ~xor_(args); }


bx_t
boolexpr::xor_(vector<bx_t> const & args)
{
    if (args.size() == 0)
        return Xor::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<Xor>(false, args);
}

bx_t
boolexpr::xor_(vector<bx_t> const && args)
{
    if (args.size() == 0)
        return Xor::identity();
    else if (args.size() == 1)
        return *args.cbegin();
    else
        return std::make_shared<Xor>(false, args);
}

bx_t
boolexpr::xor_(std::initializer_list<bx_t> const args)
{
    return xor_(vector<bx_t>(args.begin(), args.end()));
}


bx_t boolexpr::neq(vector<bx_t> const & args) { return ~eq(args); }
bx_t boolexpr::neq(vector<bx_t> const && args) { return ~eq(args); }
bx_t boolexpr::neq(std::initializer_list<bx_t> const args) { return ~eq(args); }


bx_t
boolexpr::eq(vector<bx_t> const & args)
{
    if (args.size() < 2)
        return one();
    else
        return std::make_shared<Equal>(false, args);
}

bx_t
boolexpr::eq(vector<bx_t> const && args)
{
    if (args.size() < 2)
        return one();
    else
        return std::make_shared<Equal>(false, args);
}

bx_t
boolexpr::eq(std::initializer_list<bx_t> const args)
{
    return eq(vector<bx_t>(args.begin(), args.end()));
}


bx_t
boolexpr::nimpl(bx_t const & p, bx_t const & q)
{
    return std::make_shared<NotImplies>(false, p, q);
}


bx_t
boolexpr::impl(bx_t const & p, bx_t const & q)
{
    return std::make_shared<Implies>(false, p, q);
}


bx_t
boolexpr::nite(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return std::make_shared<NotIfThenElse>(false, s, d1, d0);
}


bx_t
boolexpr::ite(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return std::make_shared<IfThenElse>(false, s, d1, d0);
}


bx_t
boolexpr::onehot0(vector<bx_t> const & args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j)
            terms[cnt++] = ~args[i] | ~args[j];
    }

    return and_(std::move(terms));
}

bx_t
boolexpr::onehot0(vector<bx_t> const && args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j)
            terms[cnt++] = ~args[i] | ~args[j];
    }

    return and_(std::move(terms));
}

bx_t
boolexpr::onehot0(std::initializer_list<bx_t> const args)
{
    return onehot0(vector<bx_t>(args.begin(), args.end()));
}


bx_t
boolexpr::onehot(vector<bx_t> const & args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2 + 1);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j)
            terms[cnt++] = ~args[i] | ~args[j];
    }

    terms[cnt++] = or_(args);

    return and_(std::move(terms));
}

bx_t
boolexpr::onehot(vector<bx_t> const && args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2 + 1);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j)
            terms[cnt++] = ~args[i] | ~args[j];
    }

    terms[cnt++] = or_(args);

    return and_(std::move(terms));
}

bx_t
boolexpr::onehot(std::initializer_list<bx_t> const args)
{
    return onehot(vector<bx_t>(args.begin(), args.end()));
}


bx_t
boolexpr::nor_s(vector<bx_t> const & args)
{ return nor(args)->simplify(); }

bx_t
boolexpr::nor_s(vector<bx_t> const && args)
{ return nor(args)->simplify(); }

bx_t
boolexpr::nor_s(std::initializer_list<bx_t> const args)
{ return nor(args)->simplify(); }

bx_t
boolexpr::or_s(vector<bx_t> const & args)
{ return or_(args)->simplify(); }

bx_t
boolexpr::or_s(vector<bx_t> const && args)
{ return or_(args)->simplify(); }

bx_t
boolexpr::or_s(std::initializer_list<bx_t> const args)
{ return or_(args)->simplify(); }

bx_t
boolexpr::nand_s(vector<bx_t> const & args)
{ return nand(args)->simplify(); }

bx_t
boolexpr::nand_s(vector<bx_t> const && args)
{ return nand(args)->simplify(); }

bx_t
boolexpr::nand_s(std::initializer_list<bx_t> const args)
{ return nand(args)->simplify(); }

bx_t
boolexpr::and_s(vector<bx_t> const & args)
{ return and_(args)->simplify(); }

bx_t
boolexpr::and_s(vector<bx_t> const && args)
{ return and_(args)->simplify(); }

bx_t
boolexpr::and_s(std::initializer_list<bx_t> const args)
{ return and_(args)->simplify(); }

bx_t
boolexpr::xnor_s(vector<bx_t> const & args)
{ return xnor(args)->simplify(); }

bx_t
boolexpr::xnor_s(vector<bx_t> const && args)
{ return xnor(args)->simplify(); }

bx_t
boolexpr::xnor_s(std::initializer_list<bx_t> const args)
{ return xnor(args)->simplify(); }

bx_t
boolexpr::xor_s(vector<bx_t> const & args)
{ return xor_(args)->simplify(); }

bx_t
boolexpr::xor_s(vector<bx_t> const && args)
{ return xor_(args)->simplify(); }

bx_t
boolexpr::xor_s(std::initializer_list<bx_t> const args)
{ return xor_(args)->simplify(); }

bx_t
boolexpr::neq_s(vector<bx_t> const & args)
{ return neq(args)->simplify(); }

bx_t
boolexpr::neq_s(vector<bx_t> const && args)
{ return neq(args)->simplify(); }

bx_t
boolexpr::neq_s(std::initializer_list<bx_t> const args)
{ return neq(args)->simplify(); }

bx_t
boolexpr::eq_s(vector<bx_t> const & args)
{ return eq(args)->simplify(); }

bx_t
boolexpr::eq_s(vector<bx_t> const && args)
{ return eq(args)->simplify(); }

bx_t
boolexpr::eq_s(std::initializer_list<bx_t> const args)
{ return eq(args)->simplify(); }

bx_t
boolexpr::nimpl_s(bx_t const & p, bx_t const & q)
{ return nimpl(p, q)->simplify(); }

bx_t
boolexpr::impl_s(bx_t const & p, bx_t const & q)
{ return impl(p, q)->simplify(); }

bx_t
boolexpr::nite_s(bx_t const & s, bx_t const & d1, bx_t const & d0)
{ return nite(s, d1, d0)->simplify(); }

bx_t
boolexpr::ite_s(bx_t const & s, bx_t const & d1, bx_t const & d0)
{ return ite(s, d1, d0)->simplify(); }


// Overloaded Operators
bx_t boolexpr::operator~(bx_t const & self) { return self->invert(); }
bx_t boolexpr::operator|(bx_t const & lhs, bx_t const & rhs) { return or_({lhs, rhs}); }
bx_t boolexpr::operator&(bx_t const & lhs, bx_t const & rhs) { return and_({lhs, rhs}); }
bx_t boolexpr::operator^(bx_t const & lhs, bx_t const & rhs) { return xor_({lhs, rhs}); }


bool
boolexpr::operator<(lit_t const & lhs, lit_t const & rhs)
{
    return lhs->ctx == rhs->ctx ? lhs->id < rhs->id
                                : lhs->ctx < rhs->ctx;
}


// String conversion
std::ostream& Zero::op_lsh(std::ostream& s) const { return s << "0"; }
std::ostream& One::op_lsh(std::ostream& s) const { return s << "1"; }
std::ostream& Logical::op_lsh(std::ostream& s) const { return s << "X"; }
std::ostream& Illogical::op_lsh(std::ostream& s) const { return s << "?"; }


std::ostream&
Complement::op_lsh(std::ostream& s) const
{
    auto self = shared_from_this();
    auto xn = std::static_pointer_cast<Complement const>(self);
    return s << "~" << xn->ctx->get_name(xn->id);
}


std::ostream&
Variable::op_lsh(std::ostream& s) const
{
    auto self = shared_from_this();
    auto x = std::static_pointer_cast<Variable const>(self);
    return s << x->ctx->get_name(x->id);
}


std::ostream&
Operator::op_lsh(std::ostream& s) const
{
    s << opname() << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0) s << ", ";
        s << args[i];
    }
    return s << ")";
}


string const Nor::opname() const { return "Nor"; };
string const Or::opname() const { return "Or"; };
string const Nand::opname() const { return "Nand"; };
string const And::opname() const { return "And"; };
string const Xnor::opname() const { return "Xnor"; };
string const Xor::opname() const { return "Xor"; };
string const Unequal::opname() const { return "Unequal"; };
string const Equal::opname() const { return "Equal"; };
string const NotImplies::opname() const { return "NotImplies"; };
string const Implies::opname() const { return "Implies"; };
string const NotIfThenElse::opname() const { return "NotIfThenElse"; };
string const IfThenElse::opname() const { return "IfThenElse"; };


string
BoolExpr::to_string() const
{
    std::ostringstream oss;
    oss << shared_from_this();
    return oss.str();
}


std::ostream&
boolexpr::operator<<(std::ostream& s, bx_t const & bx)
{ return bx->op_lsh(s); }


op_t Nor::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Nor>(false, args); }

op_t Or::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Or>(false, args); }

op_t Nand::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Nand>(false, args); }

op_t And::from_args(vector<bx_t> const && args) const
{ return std::make_shared<And>(false, args); }

op_t Xnor::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Xnor>(false, args); }

op_t Xor::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Xor>(false, args); }

op_t Unequal::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Unequal>(false, args); }

op_t Equal::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Equal>(false, args); }

op_t NotImplies::from_args(vector<bx_t> const && args) const
{ return std::make_shared<NotImplies>(false, args[0], args[1]); }

op_t Implies::from_args(vector<bx_t> const && args) const
{ return std::make_shared<Implies>(false, args[0], args[1]); }

op_t NotIfThenElse::from_args(vector<bx_t> const && args) const
{ return std::make_shared<NotIfThenElse>(false, args[0], args[1], args[2]); }

op_t IfThenElse::from_args(vector<bx_t> const && args) const
{ return std::make_shared<IfThenElse>(false, args[0], args[1], args[2]); }


// Properties
bool
Operator::is_clause() const
{
    for (bx_t const & arg : args)
        if (!IS_LIT(arg)) return false;
    return true;
}


bool Atom::is_cnf() const { return false; }
bool One::is_cnf() const { return true; }
bool Literal::is_cnf() const { return true; }
bool Operator::is_cnf() const { return false; }


bool
Or::is_cnf() const
{
    for (bx_t const & arg : args)
        if (!IS_LIT(arg))
            return false;
    return true;
}


bool
And::is_cnf() const
{
    for (bx_t const & arg : args)
        if (!IS_LIT(arg) && !(IS_OR(arg) && std::static_pointer_cast<Or const>(arg)->is_clause()))
            return false;
    return true;
}


bool Atom::is_dnf() const { return false; }
bool Zero::is_dnf() const { return true; }
bool Literal::is_dnf() const { return true; }
bool Operator::is_dnf() const { return false; }


bool
Or::is_dnf() const
{
    for (bx_t const & arg : args)
        if (!IS_LIT(arg) && !(IS_AND(arg) && std::static_pointer_cast<And const>(arg)->is_clause()))
            return false;
    return true;
}


bool
And::is_dnf() const
{
    for (bx_t const & arg : args)
        if (!IS_LIT(arg))
            return false;
    return true;
}


std::unordered_set<var_t>
BoolExpr::support() const
{
    auto self = shared_from_this();

    std::unordered_set<var_t> s;

    for (auto it = dfs_iter(self); it != dfs_iter(); ++it) {
        if (IS_VAR(*it))
            s.insert(std::static_pointer_cast<Variable const>(*it));
        else if (IS_COMP(*it))
            s.insert(std::static_pointer_cast<Variable const>(~*it));
    }

    return std::move(s);
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

    if (mod_count)
        return from_args(std::move(_args));

    return std::static_pointer_cast<Operator const>(shared_from_this());
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
