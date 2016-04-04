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
#include <cryptominisat4/cryptominisat.h>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


struct BoolExprProxy
{
    bx_t const bx;

    BoolExprProxy(bx_t const & bx): bx {bx} {}
};


template <typename T>
struct VecProxy
{
    vector<T> v;
    typename std::vector<T>::iterator it;

    VecProxy(vector<T> const & v): v {v} {}

    void iter() { it = v.begin(); }
    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == v.end()) ? nullptr : new BoolExprProxy(*it);
    }
};


template <typename T>
struct SetProxy
{
    std::unordered_set<T> s;
    typename std::unordered_set<T>::iterator it;

    SetProxy(std::unordered_set<T> const && s): s {s} {}

    void iter() { it = s.begin(); }
    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == s.end()) ? nullptr : new BoolExprProxy(*it);
    }
};


template <typename K, typename V>
struct MapProxy
{
    std::unordered_map<K, V> m;
    typename std::unordered_map<K, V>::iterator it;

    MapProxy(std::unordered_map<K, V> const & m): m {m} {}
    MapProxy(std::unordered_map<K, V> const && m): m {m} {}

    void iter() { it = m.begin(); }
    void next() { ++it; }

    BoolExprProxy * key() const
    {
        return (it == m.end()) ? nullptr : new BoolExprProxy((*it).first);
    }

    BoolExprProxy * val() const
    {
        return (it == m.end()) ? nullptr : new BoolExprProxy((*it).second);
    }
};


struct SolnProxy
{
    soln_t soln;

    SolnProxy(soln_t const && soln): soln {soln} {}
};


struct DfsIterProxy
{
    dfs_iter it;

    DfsIterProxy(bx_t const & bx): it {dfs_iter(bx)} {}

    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == dfs_iter()) ? nullptr : new BoolExprProxy(*it);
    }
};


struct SatIterProxy
{
    sat_iter it;

    SatIterProxy(bx_t const & bx): it {sat_iter(bx)} {}

    void next() { ++it; }

    MapProxy<var_t, const_t> * val() const
    {
        return (it == sat_iter()) ? nullptr : new MapProxy<var_t, const_t>(*it);
    }
};


void *
boolexpr_Context_new()
{
    return new Context();
}


void
boolexpr_Context_del(void * c_self)
{
    auto self = reinterpret_cast<Context *>(c_self);
    delete self;
}


void const *
boolexpr_Context_get_var(void * c_self, char const * c_name)
{
    auto self = reinterpret_cast<Context *>(c_self);
    std::string name { c_name };

    auto var = self->get_var(name);
    auto bx = std::static_pointer_cast<BoolExpr const>(var);
    return new BoolExprProxy(bx);
}


void
boolexpr_String_del(char const * c_str)
{
    delete c_str;
}


void
boolexpr_Vec_del(void const * c_self)
{
    auto self = reinterpret_cast<VecProxy<bx_t> const *>(c_self);
    delete self;
}


void
boolexpr_Vec_iter(void * c_self)
{
    auto self = reinterpret_cast<VecProxy<bx_t> *>(c_self);
    self->iter();
}


void
boolexpr_Vec_next(void * c_self)
{
    auto self = reinterpret_cast<VecProxy<bx_t> *>(c_self);
    self->next();
}


void const *
boolexpr_Vec_val(void const * c_self)
{
    auto self = reinterpret_cast<VecProxy<bx_t> const *>(c_self);
    return self->val();
}


void
boolexpr_VarSet_del(void const * c_self)
{
    auto self = reinterpret_cast<SetProxy<var_t> const *>(c_self);
    delete self;
}


void
boolexpr_VarSet_iter(void * c_self)
{
    auto self = reinterpret_cast<SetProxy<var_t> *>(c_self);
    self->iter();
}


void
boolexpr_VarSet_next(void * c_self)
{
    auto self = reinterpret_cast<SetProxy<var_t> *>(c_self);
    self->next();
}


void const *
boolexpr_VarSet_val(void const * c_self)
{
    auto self = reinterpret_cast<SetProxy<var_t> const *>(c_self);
    return self->val();
}


void
boolexpr_Point_del(void const * c_self)
{
    auto self = reinterpret_cast<MapProxy<var_t, const_t> const *>(c_self);
    delete self;
}


void
boolexpr_Point_iter(void * c_self)
{
    auto self = reinterpret_cast<MapProxy<var_t, const_t> *>(c_self);
    self->iter();
}


void
boolexpr_Point_next(void * c_self)
{
    auto self = reinterpret_cast<MapProxy<var_t, const_t> *>(c_self);
    self->next();
}


void const *
boolexpr_Point_key(void const * c_self)
{
    auto self = reinterpret_cast<MapProxy<var_t, const_t> const *>(c_self);
    return self->key();
}


void const *
boolexpr_Point_val(void const * c_self)
{
    auto self = reinterpret_cast<MapProxy<var_t, const_t> const *>(c_self);
    return self->val();
}


void
boolexpr_Soln_del(void const * c_self)
{
    auto self = reinterpret_cast<SolnProxy const *>(c_self);
    delete self;
}


bool
boolexpr_Soln_first(void const * c_self)
{
    auto self = reinterpret_cast<SolnProxy const *>(c_self);
    return self->soln.first;
}


void const *
boolexpr_Soln_second(void const * c_self)
{
    auto self = reinterpret_cast<SolnProxy const *>(c_self);
    auto point = *(self->soln.second);
    return new MapProxy<var_t, const_t>(std::move(point));
}


void const *
boolexpr_SatIter_new(void const * c_bxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(c_bxp);
    return new SatIterProxy(bxp->bx);
}


void
boolexpr_SatIter_del(void const * c_self)
{
    auto self = reinterpret_cast<SatIterProxy const *>(c_self);
    delete self;
}


void
boolexpr_SatIter_next(void * c_self)
{
    auto self = reinterpret_cast<SatIterProxy *>(c_self);
    self->next();
}


void const *
boolexpr_SatIter_val(void const * c_self)
{
    auto self = reinterpret_cast<SatIterProxy const *>(c_self);
    return self->val();
}


void const *
boolexpr_DfsIter_new(void const * c_bxp)
{
    auto bxp = reinterpret_cast<BoolExprProxy const *>(c_bxp);
    return new DfsIterProxy(bxp->bx);
}


void
boolexpr_DfsIter_del(void const * c_self)
{
    auto self = reinterpret_cast<DfsIterProxy const *>(c_self);
    delete self;
}


void
boolexpr_DfsIter_next(void * c_self)
{
    auto self = reinterpret_cast<DfsIterProxy *>(c_self);
    self->next();
}


void const *
boolexpr_DfsIter_val(void const * c_self)
{
    auto self = reinterpret_cast<DfsIterProxy const *>(c_self);
    return self->val();
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
boolexpr_not(void const * c_arg)
{
    auto arg = reinterpret_cast<BoolExprProxy const *>(c_arg);
    return new BoolExprProxy(~arg->bx);
}


static vector<bx_t>
_convert_args(size_t n, void const * c_args[])
{
    vector<bx_t> _args(n);
    for (size_t i = 0; i < n; ++i) {
        auto arg = reinterpret_cast<BoolExprProxy const *>(c_args[i]);
        _args[i] = arg->bx;
    }
    return std::move(_args);
}


void const *
boolexpr_nor(size_t n, void const * c_args[])
{ return new BoolExprProxy(nor(_convert_args(n, c_args))); }

void const *
boolexpr_or(size_t n, void const * c_args[])
{ return new BoolExprProxy(or_(_convert_args(n, c_args))); }

void const *
boolexpr_nand(size_t n, void const * c_args[])
{ return new BoolExprProxy(nand(_convert_args(n, c_args))); }

void const *
boolexpr_and(size_t n, void const * c_args[])
{ return new BoolExprProxy(and_(_convert_args(n, c_args))); }

void const *
boolexpr_xnor(size_t n, void const * c_args[])
{ return new BoolExprProxy(xnor(_convert_args(n, c_args))); }

void const *
boolexpr_xor(size_t n, void const * c_args[])
{ return new BoolExprProxy(xor_(_convert_args(n, c_args))); }

void const *
boolexpr_neq(size_t n, void const * c_args[])
{ return new BoolExprProxy(neq(_convert_args(n, c_args))); }

void const *
boolexpr_eq(size_t n, void const * c_args[])
{ return new BoolExprProxy(eq(_convert_args(n, c_args))); }

void const *
boolexpr_impl(void const * c_p, void const * c_q)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(c_p);
    auto q = reinterpret_cast<BoolExprProxy const *>(c_q);
    return new BoolExprProxy(impl(p->bx, q->bx));
}

void const *
boolexpr_ite(void const * c_s, void const * c_d1, void const * c_d0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(c_s);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(c_d1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(c_d0);
    return new BoolExprProxy(ite(s->bx, d1->bx, d0->bx));
}


void const *
boolexpr_nor_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(nor_s(_convert_args(n, c_args))); }

void const *
boolexpr_or_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(or_s(_convert_args(n, c_args))); }

void const *
boolexpr_nand_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(nand_s(_convert_args(n, c_args))); }

void const *
boolexpr_and_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(and_s(_convert_args(n, c_args))); }

void const *
boolexpr_xnor_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(xnor_s(_convert_args(n, c_args))); }

void const *
boolexpr_xor_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(xor_s(_convert_args(n, c_args))); }

void const *
boolexpr_neq_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(neq_s(_convert_args(n, c_args))); }

void const *
boolexpr_eq_s(size_t n, void const * c_args[])
{ return new BoolExprProxy(eq_s(_convert_args(n, c_args))); }

void const *
boolexpr_impl_s(void const * c_p, void const * c_q)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(c_p);
    auto q = reinterpret_cast<BoolExprProxy const *>(c_q);
    return new BoolExprProxy(impl_s(p->bx, q->bx));
}

void const *
boolexpr_ite_s(void const * c_s, void const * c_d1, void const * c_d0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(c_s);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(c_d1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(c_d0);
    return new BoolExprProxy(ite_s(s->bx, d1->bx, d0->bx));
}


void
boolexpr_BoolExpr_del(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    delete self;
}


uint8_t
boolexpr_BoolExpr_kind(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return static_cast<uint8_t>(self->bx->kind);
}


char const *
boolexpr_BoolExpr_to_string(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto str = self->bx->to_string();
    auto c_str = new char [str.length() + 1];
    std::strcpy(c_str, str.c_str());
    return c_str;
}


uint32_t
boolexpr_BoolExpr_depth(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->depth();
}


uint32_t
boolexpr_BoolExpr_size(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->size();
}


uint32_t
boolexpr_BoolExpr_atom_count(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->atom_count();
}


uint32_t
boolexpr_BoolExpr_op_count(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->op_count();
}


bool
boolexpr_BoolExpr_is_cnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->is_cnf();
}


bool
boolexpr_BoolExpr_is_dnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->is_dnf();
}


void const *
boolexpr_BoolExpr_pushdown_not(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->pushdown_not());
}


void const *
boolexpr_BoolExpr_simplify(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->simplify());
}


void const *
boolexpr_BoolExpr_to_binop(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_binop());
}


void const *
boolexpr_BoolExpr_to_latop(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_latop());
}


void const *
boolexpr_BoolExpr_tseytin(void const * c_self, void * c_ctx, char const * c_auxvarname)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto ctx = reinterpret_cast<Context *>(c_ctx);
    std::string auxvarname { c_auxvarname };
    return new BoolExprProxy(self->bx->tseytin(*ctx, auxvarname));
}


void const *
boolexpr_BoolExpr_compose(void const * c_self, int n, void const ** c_varps, void const ** c_bxps)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto var2bx = var2bx_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(c_varps[i]);
        auto bxp = reinterpret_cast<BoolExprProxy const *>(c_bxps[i]);
        auto var = std::static_pointer_cast<Variable const>(varp->bx);
        auto bx = bxp->bx;
        var2bx.insert({var, bx});
    }
    return new BoolExprProxy(self->bx->compose(var2bx));
}


void const *
boolexpr_BoolExpr_restrict(void const * c_self, int n, void const ** c_varps, void const ** c_constps)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto point = point_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(c_varps[i]);
        auto constp = reinterpret_cast<BoolExprProxy const *>(c_constps[i]);
        auto var = std::static_pointer_cast<Variable const>(varp->bx);
        auto const_ = std::static_pointer_cast<Constant const>(constp->bx);
        point.insert({var, const_});
    }
    return new BoolExprProxy(self->bx->restrict_(point));
}


void const *
boolexpr_BoolExpr_sat(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new SolnProxy(self->bx->sat());
}


void const *
boolexpr_BoolExpr_to_cnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_cnf());
}


void const *
boolexpr_BoolExpr_to_dnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_dnf());
}


void const *
boolexpr_BoolExpr_to_nnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_nnf());
}


bool
boolexpr_BoolExpr_equiv(void const * c_self, void const * c_other)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto other = reinterpret_cast<BoolExprProxy const *>(c_other);
    return self->bx->equiv(other->bx);
}


void const *
boolexpr_BoolExpr_support(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new SetProxy<var_t>(self->bx->support());
}


void *
boolexpr_Literal_ctx(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto lit = std::static_pointer_cast<Literal const>(self->bx);
    return lit->ctx;
}


uint32_t
boolexpr_Literal_id(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto lit = std::static_pointer_cast<Literal const>(self->bx);
    return lit->id;
}


bool
boolexpr_Operator_simple(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto op = std::static_pointer_cast<Operator const>(self->bx);
    return op->simple;
}


void const *
boolexpr_Operator_args(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto op = std::static_pointer_cast<Operator const>(self->bx);
    return new VecProxy<bx_t>(op->args);
}


bool
boolexpr_Operator_is_clause(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto op = std::static_pointer_cast<Operator const>(self->bx);
    return op->is_clause();
}
