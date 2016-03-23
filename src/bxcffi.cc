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
boolexpr_Context_del(void * vctx)
{
    auto ctx = reinterpret_cast<Context *>(vctx);
    delete ctx;
}


void const *
boolexpr_Context_get_var(void * vctx, char const * name)
{
    auto ctx = reinterpret_cast<Context *>(vctx);
    std::string _name { name };

    auto var = ctx->get_var(_name);
    auto x = std::static_pointer_cast<BoolExpr const>(var);
    return new BoolExprProxy(x);
}


struct StringProxy {

    char * str;

    StringProxy(string const & s)
    {
        str = new char [s.length() + 1];
        std::strcpy(str, s.c_str());
    }

    ~StringProxy()
    {
        delete [] str;
    }
};


void
boolexpr_StringProxy_del(void const *vsp)
{
    auto sp = reinterpret_cast<StringProxy const *>(vsp);
    delete sp;
}


char const *
boolexpr_StringProxy_str(void const *vsp)
{
    auto sp = reinterpret_cast<StringProxy const *>(vsp);
    return sp->str;
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
boolexpr_not(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(~bxp->bx);
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
boolexpr_impl(void const * vp, void const * vq)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(vp);
    auto q = reinterpret_cast<BoolExprProxy const *>(vq);
    return new BoolExprProxy(impl(p->bx, q->bx));
}

void const *
boolexpr_ite(void const * vs, void const * vd1, void const * vd0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(vs);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(vd1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(vd0);
    return new BoolExprProxy(ite(s->bx, d1->bx, d0->bx));
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
boolexpr_impl_s(void const * vp, void const * vq)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(vp);
    auto q = reinterpret_cast<BoolExprProxy const *>(vq);
    return new BoolExprProxy(impl_s(p->bx, q->bx));
}

void const *
boolexpr_ite_s(void const * vs, void const * vd1, void const * vd0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(vs);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(vd1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(vd0);
    return new BoolExprProxy(ite_s(s->bx, d1->bx, d0->bx));
}


void
boolexpr_BoolExpr_del(void const *vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    delete bxp;
}


uint32_t
boolexpr_BoolExpr_kind(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return static_cast<uint32_t>(bxp->bx->kind);
}


void const *
boolexpr_BoolExpr_to_string(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new StringProxy(bxp->bx->to_string());
}


uint32_t
boolexpr_BoolExpr_depth(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->depth();
}


uint32_t
boolexpr_BoolExpr_size(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->size();
}


uint32_t
boolexpr_BoolExpr_atom_count(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->atom_count();
}


uint32_t
boolexpr_BoolExpr_op_count(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->op_count();
}


bool
boolexpr_BoolExpr_is_cnf(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->is_cnf();
}


bool
boolexpr_BoolExpr_is_dnf(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return bxp->bx->is_dnf();
}


void const *
boolexpr_BoolExpr_pushdown_not(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->pushdown_not());
}


void const *
boolexpr_BoolExpr_simplify(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->simplify());
}


void const *
boolexpr_BoolExpr_to_binop(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->to_binop());
}


void const *
boolexpr_BoolExpr_to_latop(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->to_latop());
}


void const *
boolexpr_BoolExpr_tseytin(void const * vbxp, void * vctx, char const * auxvarname)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    auto ctx = reinterpret_cast<Context *>(vctx);
    std::string _auxvarname { auxvarname };
    return new BoolExprProxy(bxp->bx->tseytin(*ctx, _auxvarname));
}


void const *
boolexpr_BoolExpr_compose(void const * vbxp, int n, void const ** vvars, void const ** vbxs)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    auto var2bx = var2bx_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(vvars[i]);
        auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxs[i]);
        var2bx.insert({std::static_pointer_cast<Variable const>(varp->bx), bxp->bx});
    }
    return new BoolExprProxy(bxp->bx->compose(var2bx));
}


void const *
boolexpr_BoolExpr_restrict(void const * vbxp, int n, void const ** vvars, void const ** vconsts)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    auto point = point_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(vvars[i]);
        auto constp = reinterpret_cast<BoolExprProxy const *>(vconsts[i]);
        point.insert({std::static_pointer_cast<Variable const>(varp->bx),
                      std::static_pointer_cast<Constant const>(bxp->bx)});
    }
    return new BoolExprProxy(bxp->bx->restrict_(point));
}


void const *
boolexpr_BoolExpr_to_cnf(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->to_cnf());
}


void const *
boolexpr_BoolExpr_to_dnf(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->to_dnf());
}


void const *
boolexpr_BoolExpr_to_nnf(void const * vbxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(vbxp);
    return new BoolExprProxy(bxp->bx->to_nnf());
}


bool
boolexpr_BoolExpr_equiv(void const * vbxp1, void const * vbxp2)
{
    auto bxp1 = reinterpret_cast<BoolExprProxy const *>(vbxp1);
    auto bxp2 = reinterpret_cast<BoolExprProxy const *>(vbxp2);
    return bxp1->bx->equiv(bxp2->bx);
}
