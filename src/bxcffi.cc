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


#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


struct BoolExprProxy {
    bx_t const bx;

    BoolExprProxy(bx_t const & bx): bx {bx} {}
    ~BoolExprProxy() {}
};


void *
boolexpr_Context_new()
{
    return new Context();
}


void
boolexpr_Context_del(void * ctx)
{
    auto _ctx = reinterpret_cast<Context *>(ctx);
    delete _ctx;
}


void const *
boolexpr_Context_get_var(void * ctx, char const * name)
{
    auto _ctx = reinterpret_cast<Context *>(ctx);
    std::string _name { name };

    auto var = _ctx->get_var(_name);
    auto x = std::static_pointer_cast<BoolExpr const>(var);
    return new BoolExprProxy(x);
}


void const *
boolexpr_zero()
{ return new BoolExprProxy(zero()); }

void const *
boolexpr_one()
{ return new BoolExprProxy(one()); }

void const *
boolexpr_logical()
{ return new BoolExprProxy(logical()); }

void const *
boolexpr_illogical()
{ return new BoolExprProxy(illogical()); }


void const *
boolexpr_not(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(~bx);
}


static vector<bx_t>
_convert_args(uint32_t n, void const * args[])
{
    vector<bx_t> _args(n);
    for (uint32_t i = 0u; i < n; ++i)
        _args[i] = reinterpret_cast<BoolExprProxy const *>(args[i])->bx;
    return std::move(_args);
}


void const *
boolexpr_nor(uint32_t n, void const * args[])
{ return new BoolExprProxy(nor(_convert_args(n, args))); }

void const *
boolexpr_or(uint32_t n, void const * args[])
{ return new BoolExprProxy(or_(_convert_args(n, args))); }

void const *
boolexpr_nand(uint32_t n, void const * args[])
{ return new BoolExprProxy(nand(_convert_args(n, args))); }

void const *
boolexpr_and(uint32_t n, void const * args[])
{ return new BoolExprProxy(and_(_convert_args(n, args))); }

void const *
boolexpr_xnor(uint32_t n, void const * args[])
{ return new BoolExprProxy(xnor(_convert_args(n, args))); }

void const *
boolexpr_xor(uint32_t n, void const * args[])
{ return new BoolExprProxy(xor_(_convert_args(n, args))); }

void const *
boolexpr_neq(uint32_t n, void const * args[])
{ return new BoolExprProxy(neq(_convert_args(n, args))); }

void const *
boolexpr_eq(uint32_t n, void const * args[])
{ return new BoolExprProxy(eq(_convert_args(n, args))); }

void const *
boolexpr_impl(void const * p, void const * q)
{
    auto _p = reinterpret_cast<BoolExprProxy const *>(p)->bx;
    auto _q = reinterpret_cast<BoolExprProxy const *>(q)->bx;
    return new BoolExprProxy(impl(_p, _q));
}

void const *
boolexpr_ite(void const * s, void const * d1, void const * d0)
{
    auto _s = reinterpret_cast<BoolExprProxy const *>(s)->bx;
    auto _d1 = reinterpret_cast<BoolExprProxy const *>(d1)->bx;
    auto _d0 = reinterpret_cast<BoolExprProxy const *>(d0)->bx;
    return new BoolExprProxy(ite(_s, _d1, _d0));
}


void const *
boolexpr_nor_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(nor_s(_convert_args(n, args))); }

void const *
boolexpr_or_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(or_s(_convert_args(n, args))); }

void const *
boolexpr_nand_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(nand_s(_convert_args(n, args))); }

void const *
boolexpr_and_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(and_s(_convert_args(n, args))); }

void const *
boolexpr_xnor_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(xnor_s(_convert_args(n, args))); }

void const *
boolexpr_xor_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(xor_s(_convert_args(n, args))); }

void const *
boolexpr_neq_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(neq_s(_convert_args(n, args))); }

void const *
boolexpr_eq_s(uint32_t n, void const * args[])
{ return new BoolExprProxy(eq_s(_convert_args(n, args))); }

void const *
boolexpr_impl_s(void const * p, void const * q)
{
    auto _p = reinterpret_cast<BoolExprProxy const *>(p)->bx;
    auto _q = reinterpret_cast<BoolExprProxy const *>(q)->bx;
    return new BoolExprProxy(impl_s(_p, _q));
}

void const *
boolexpr_ite_s(void const * s, void const * d1, void const * d0)
{
    auto _s = reinterpret_cast<BoolExprProxy const *>(s)->bx;
    auto _d1 = reinterpret_cast<BoolExprProxy const *>(d1)->bx;
    auto _d0 = reinterpret_cast<BoolExprProxy const *>(d0)->bx;
    return new BoolExprProxy(ite_s(_s, _d1, _d0));
}


void
boolexpr_BoolExpr_del(void const *bxp)
{
    auto _bxp = reinterpret_cast<BoolExprProxy const *>(bxp);
    delete _bxp;
}


uint32_t
boolexpr_BoolExpr_kind(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return static_cast<uint32_t>(bx->kind);
}


char const *
boolexpr_BoolExpr_to_string(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    auto str = bx->to_string();
    char * cstr = new char [str.length()+1];
    std::strcpy(cstr, str.c_str());
    return cstr;
}


void
boolexpr_string_del(char const * b)
{
    delete [] b;
}


uint32_t
boolexpr_BoolExpr_depth(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->depth();
}


uint32_t
boolexpr_BoolExpr_size(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->size();
}


uint32_t
boolexpr_BoolExpr_atom_count(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->atom_count();
}


uint32_t
boolexpr_BoolExpr_op_count(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->op_count();
}


bool
boolexpr_BoolExpr_is_cnf(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->is_cnf();
}


bool
boolexpr_BoolExpr_is_dnf(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return bx->is_dnf();
}


void const *
boolexpr_BoolExpr_pushdown_not(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->pushdown_not());
}


void const *
boolexpr_BoolExpr_simplify(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->simplify());
}


void const *
boolexpr_BoolExpr_to_binop(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->to_binop());
}


void const *
boolexpr_BoolExpr_to_latop(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->to_latop());
}


void const *
boolexpr_BoolExpr_tseytin(void const * bxp, void * ctx, char const * auxvarname)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    auto _ctx = reinterpret_cast<Context *>(ctx);
    std::string _auxvarname { auxvarname };
    return new BoolExprProxy(bx->tseytin(*_ctx, _auxvarname));
}


void const *
boolexpr_BoolExpr_to_cnf(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->to_cnf());
}


void const *
boolexpr_BoolExpr_to_dnf(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->to_dnf());
}


void const *
boolexpr_BoolExpr_to_nnf(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(bx->to_nnf());
}


bool
boolexpr_BoolExpr_equiv(void const * bxp1, void const * bxp2)
{
    auto bx1 = reinterpret_cast<BoolExprProxy const *>(bxp1)->bx;
    auto bx2 = reinterpret_cast<BoolExprProxy const *>(bxp2)->bx;
    return bx1->equiv(bx2);
}
