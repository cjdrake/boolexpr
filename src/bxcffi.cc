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
boolexpr_Context()
{
    return new Context();
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
{
    return new BoolExprProxy(zero());
}


void const *
boolexpr_one()
{
    return new BoolExprProxy(one());
}


void const *
boolexpr_not(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;
    return new BoolExprProxy(~bx);
}


void const *
boolexpr_or(uint32_t n, void const * args[])
{
    vector<bx_t> _args;

    for (uint32_t i = 0u; i < n; ++i) {
        auto _arg = reinterpret_cast<BoolExprProxy const *>(args[i])->bx;
        _args.push_back(_arg);
    }

    return new BoolExprProxy(or_(_args));
}


void const *
boolexpr_and(uint32_t n, void const * args[])
{
    vector<bx_t> _args;

    for (uint32_t i = 0u; i < n; ++i) {
        auto _arg = reinterpret_cast<BoolExprProxy const *>(args[i])->bx;
        _args.push_back(_arg);
    }

    return new BoolExprProxy(and_(_args));
}


void const *
boolexpr_xor(uint32_t n, void const * args[])
{
    vector<bx_t> _args;

    for (uint32_t i = 0u; i < n; ++i) {
        auto _arg = reinterpret_cast<BoolExprProxy const *>(args[i])->bx;
        _args.push_back(_arg);
    }

    return new BoolExprProxy(xor_(_args));
}


void const *
boolexpr_eq(uint32_t n, void const * args[])
{
    vector<bx_t> _args;

    for (uint32_t i = 0u; i < n; ++i) {
        auto _arg = reinterpret_cast<BoolExprProxy const *>(args[i])->bx;
        _args.push_back(_arg);
    }

    return new BoolExprProxy(eq(_args));
}


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
    char * cstr = new char [str.length() + 1];
    std::strcpy(cstr, str.c_str());
    return cstr;
}


uint32_t
boolexpr_BoolExpr_depth(void const * bxp)
{
    auto bx = reinterpret_cast<BoolExprProxy const *>(bxp)->bx;

    return bx->depth();
}
