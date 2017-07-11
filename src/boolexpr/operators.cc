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


using std::initializer_list;
using std::make_shared;
using std::static_pointer_cast;
using std::vector;


namespace boolexpr {


bx_t
nor(vector<bx_t> const & args) {
    return ~or_(args);
}


bx_t
nor(vector<bx_t> const && args) {
    return ~or_(args);
}


bx_t
nor(initializer_list<bx_t> const args) {
    return ~or_(args);
}


bx_t
or_(vector<bx_t> const & args)
{
    if (args.size() == 0) {
        return Or::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<Or>(false, args);
    }
}


bx_t
or_(vector<bx_t> const && args)
{
    if (args.size() == 0) {
        return Or::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<Or>(false, args);
    }
}


bx_t
or_(initializer_list<bx_t> const args)
{
    return or_(vector<bx_t>(args.begin(), args.end()));
}


bx_t
nand(vector<bx_t> const & args)
{
    return ~and_(args);
}


bx_t
nand(vector<bx_t> const && args)
{
    return ~and_(args);
}


bx_t
nand(initializer_list<bx_t> const args)
{
    return ~and_(args);
}


bx_t
and_(vector<bx_t> const & args)
{
    if (args.size() == 0) {
        return And::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<And>(false, args);
    }
}


bx_t
and_(vector<bx_t> const && args)
{
    if (args.size() == 0) {
        return And::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<And>(false, args);
    }
}


bx_t
and_(initializer_list<bx_t> const args)
{
    return and_(vector<bx_t>(args.begin(), args.end()));
}


bx_t
xnor(vector<bx_t> const & args)
{
    return ~xor_(args);
}


bx_t
xnor(vector<bx_t> const && args)
{
    return ~xor_(args);
}


bx_t
xnor(initializer_list<bx_t> const args)
{
    return ~xor_(args);
}


bx_t
xor_(vector<bx_t> const & args)
{
    if (args.size() == 0) {
        return Xor::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<Xor>(false, args);
    }
}


bx_t
xor_(vector<bx_t> const && args)
{
    if (args.size() == 0) {
        return Xor::identity();
    }
    else if (args.size() == 1) {
        return *args.cbegin();
    }
    else {
        return make_shared<Xor>(false, args);
    }
}


bx_t
xor_(initializer_list<bx_t> const args)
{
    return xor_(vector<bx_t>(args.begin(), args.end()));
}


bx_t
neq(vector<bx_t> const & args)
{
    return ~eq(args);
}


bx_t
neq(vector<bx_t> const && args)
{
    return ~eq(args);
}


bx_t
neq(initializer_list<bx_t> const args)
{
    return ~eq(args);
}


bx_t
eq(vector<bx_t> const & args)
{
    if (args.size() < 2) {
        return one();
    }
    else {
        return make_shared<Equal>(false, args);
    }
}


bx_t
eq(vector<bx_t> const && args)
{
    if (args.size() < 2) {
        return one();
    }
    else {
        return make_shared<Equal>(false, args);
    }
}


bx_t
eq(initializer_list<bx_t> const args)
{
    return eq(vector<bx_t>(args.begin(), args.end()));
}


bx_t
nimpl(bx_t const & p, bx_t const & q)
{
    return make_shared<NotImplies>(false, p, q);
}


bx_t
impl(bx_t const & p, bx_t const & q)
{
    return make_shared<Implies>(false, p, q);
}


bx_t
nite(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return make_shared<NotIfThenElse>(false, s, d1, d0);
}


bx_t
ite(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return make_shared<IfThenElse>(false, s, d1, d0);
}


bx_t
onehot0(vector<bx_t> const & args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] | ~args[j];
        }
    }

    return and_(std::move(terms));
}


bx_t
onehot0(vector<bx_t> const && args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] | ~args[j];
        }
    }

    return and_(std::move(terms));
}


bx_t
onehot0(initializer_list<bx_t> const args)
{
    return onehot0(vector<bx_t>(args.begin(), args.end()));
}


bx_t
onehot(vector<bx_t> const & args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2 + 1);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] | ~args[j];
        }
    }

    terms[cnt++] = or_(args);

    return and_(std::move(terms));
}


bx_t
onehot(vector<bx_t> const && args)
{
    size_t n = args.size();
    vector<bx_t> terms(n * (n-1) / 2 + 1);

    size_t cnt = 0;
    for (size_t i = 0; i < (n-1); ++i) {
        for (size_t j = i+1; j < n; ++j) {
            terms[cnt++] = ~args[i] | ~args[j];
        }
    }

    terms[cnt++] = or_(args);

    return and_(std::move(terms));
}


bx_t
onehot(initializer_list<bx_t> const args)
{
    return onehot(vector<bx_t>(args.begin(), args.end()));
}


bx_t
nor_s(vector<bx_t> const & args)
{
    return nor(args)->simplify();
}


bx_t
nor_s(vector<bx_t> const && args)
{
    return nor(args)->simplify();
}


bx_t
nor_s(initializer_list<bx_t> const args)
{
    return nor(args)->simplify();
}


bx_t
or_s(vector<bx_t> const & args)
{
    return or_(args)->simplify();
}


bx_t
or_s(vector<bx_t> const && args)
{
    return or_(args)->simplify();
}


bx_t
or_s(initializer_list<bx_t> const args)
{
    return or_(args)->simplify();
}


bx_t
nand_s(vector<bx_t> const & args)
{
    return nand(args)->simplify();
}


bx_t
nand_s(vector<bx_t> const && args)
{
    return nand(args)->simplify();
}


bx_t
nand_s(initializer_list<bx_t> const args)
{
    return nand(args)->simplify();
}


bx_t
and_s(vector<bx_t> const & args)
{
    return and_(args)->simplify();
}


bx_t
and_s(vector<bx_t> const && args)
{
    return and_(args)->simplify();
}


bx_t
and_s(initializer_list<bx_t> const args)
{
    return and_(args)->simplify();
}


bx_t
xnor_s(vector<bx_t> const & args)
{
    return xnor(args)->simplify();
}


bx_t
xnor_s(vector<bx_t> const && args)
{
    return xnor(args)->simplify();
}


bx_t
xnor_s(initializer_list<bx_t> const args)
{
    return xnor(args)->simplify();
}


bx_t
xor_s(vector<bx_t> const & args)
{
    return xor_(args)->simplify();
}


bx_t
xor_s(vector<bx_t> const && args)
{
    return xor_(args)->simplify();
}


bx_t
xor_s(initializer_list<bx_t> const args)
{
    return xor_(args)->simplify();
}


bx_t
neq_s(vector<bx_t> const & args)
{
    return neq(args)->simplify();
}


bx_t
neq_s(vector<bx_t> const && args)
{
    return neq(args)->simplify();
}


bx_t
neq_s(initializer_list<bx_t> const args)
{
    return neq(args)->simplify();
}


bx_t
eq_s(vector<bx_t> const & args)
{
    return eq(args)->simplify();
}


bx_t
eq_s(vector<bx_t> const && args)
{
    return eq(args)->simplify();
}


bx_t
eq_s(initializer_list<bx_t> const args)
{
    return eq(args)->simplify();
}


bx_t
nimpl_s(bx_t const & p, bx_t const & q)
{
    return nimpl(p, q)->simplify();
}


bx_t
impl_s(bx_t const & p, bx_t const & q)
{
    return impl(p, q)->simplify();
}


bx_t
nite_s(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return nite(s, d1, d0)->simplify();
}


bx_t
ite_s(bx_t const & s, bx_t const & d1, bx_t const & d0)
{
    return ite(s, d1, d0)->simplify();
}


bx_t
operator~(bx_t const & self)
{
    return self->invert();
}


bx_t
operator|(bx_t const & lhs, bx_t const & rhs)
{
    return or_({lhs, rhs});
}


bx_t
operator&(bx_t const & lhs, bx_t const & rhs)
{
    return and_({lhs, rhs});
}


bx_t
operator^(bx_t const & lhs, bx_t const & rhs)
{
    return xor_({lhs, rhs});
}


lit_t
abs(lit_t const & self)
{
    return self->abs();
}


lit_t
Complement::abs() const
{
    return ctx->get_lit(id + 1);
}


lit_t
Variable::abs() const
{
    return static_pointer_cast<Literal const>(shared_from_this());
}


bool
operator<(lit_t const & lhs, lit_t const & rhs)
{
    return (lhs->ctx == rhs->ctx) ? lhs->id < rhs->id
                                  : lhs->ctx < rhs->ctx;
}


}  // namespace boolexpr
