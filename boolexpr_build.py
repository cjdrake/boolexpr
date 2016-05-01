# Copyright 2016 Chris Drake
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import os
from os.path import join
from glob import glob

import cffi


# Add file from boolexpr
INCLUDE_DIRS = ["include"]
SOURCES = glob(join("src", "*.cc"))


# Add files from CryptoMiniSat 4.5.3
CMSAT = join("third_party", "cryptominisat")

INCLUDE_DIRS += [
    join(CMSAT),
    join(CMSAT, "include"),
]

SOURCES += [
    join(CMSAT, "cmsat4-src", "GitSHA1.cpp"),
    join(CMSAT, "cmsat4-src", "sql_tablestructure.cpp"),
    join(CMSAT, "src", "bva.cpp"),
    join(CMSAT, "src", "calcdefpolars.cpp"),
    join(CMSAT, "src", "clauseallocator.cpp"),
    join(CMSAT, "src", "clausecleaner.cpp"),
    join(CMSAT, "src", "clausedumper.cpp"),
    join(CMSAT, "src", "clauseusagestats.cpp"),
    join(CMSAT, "src", "cleaningstats.cpp"),
    join(CMSAT, "src", "cnf.cpp"),
    join(CMSAT, "src", "compfinder.cpp"),
    join(CMSAT, "src", "comphandler.cpp"),
    join(CMSAT, "src", "completedetachreattacher.cpp"),
    join(CMSAT, "src", "cryptominisat.cpp"),
    join(CMSAT, "src", "datasync.cpp"),
    join(CMSAT, "src", "distiller.cpp"),
    join(CMSAT, "src", "features.cpp"),
    join(CMSAT, "src", "features_calc.cpp"),
    join(CMSAT, "src", "features_to_reconf.cpp"),
    join(CMSAT, "src", "gatefinder.cpp"),
    join(CMSAT, "src", "hyperengine.cpp"),
    join(CMSAT, "src", "implcache.cpp"),
    join(CMSAT, "src", "intree.cpp"),
    join(CMSAT, "src", "occsimplifier.cpp"),
    join(CMSAT, "src", "prober.cpp"),
    join(CMSAT, "src", "propengine.cpp"),
    join(CMSAT, "src", "reducedb.cpp"),
    join(CMSAT, "src", "sccfinder.cpp"),
    join(CMSAT, "src", "searcher.cpp"),
    join(CMSAT, "src", "solutionextender.cpp"),
    join(CMSAT, "src", "solver.cpp"),
    join(CMSAT, "src", "solverconf.cpp"),
    join(CMSAT, "src", "sqlstats.cpp"),
    join(CMSAT, "src", "stamp.cpp"),
    join(CMSAT, "src", "strengthener.cpp"),
    join(CMSAT, "src", "subsumeimplicit.cpp"),
    join(CMSAT, "src", "subsumestrengthen.cpp"),
    join(CMSAT, "src", "varreplacer.cpp"),
]


# C Foreign Function Interface

HEADER = """

typedef char const * const STRING;
typedef void * const CONTEXT;
typedef void const * const BX;
typedef void const * const * const BXS;
typedef void const * const * const VARS;
typedef void const * const * const CONSTS;
typedef void * const VEC;
typedef void * const VARSET;
typedef void * const POINT;
typedef void * const SOLN;
typedef void * const SAT_ITER;
typedef void * const DFS_ITER;
typedef void * const DOM_ITER;
typedef void * const CF_ITER;

enum Kind {
    ZERO  = 0x00,   // 0 0000
    ONE   = 0x01,   // 0 0001
    LOG   = 0x04,   // 0 0100
    ILL   = 0x06,   // 0 0110
    COMP  = 0x08,   // 0 1000
    VAR   = 0x09,   // 0 1001
    NOR   = 0x10,   // 1 0000
    OR    = 0x11,   // 1 0001
    NAND  = 0x12,   // 1 0010
    AND   = 0x13,   // 1 0011
    XNOR  = 0x14,   // 1 0100
    XOR   = 0x15,   // 1 0101
    NEQ   = 0x16,   // 1 0110
    EQ    = 0x17,   // 1 0111
    NIMPL = 0x18,   // 1 1000
    IMPL  = 0x19,   // 1 1001
    NITE  = 0x1A,   // 1 1010
    ITE   = 0x1B,   // 1 1011
};

CONTEXT boolexpr_Context_new(void);
void boolexpr_Context_del(CONTEXT);
BX boolexpr_Context_get_var(CONTEXT, STRING);

void boolexpr_String_del(STRING);

void boolexpr_Vec_del(VEC);
void boolexpr_Vec_iter(VEC);
void boolexpr_Vec_next(VEC);
BX const boolexpr_Vec_val(VEC);

void boolexpr_VarSet_del(VARSET);
void boolexpr_VarSet_iter(VARSET);
void boolexpr_VarSet_next(VARSET);
BX const boolexpr_VarSet_val(VARSET);

void boolexpr_Point_del(POINT);
void boolexpr_Point_iter(POINT);
void boolexpr_Point_next(POINT);
BX const boolexpr_Point_key(POINT);
BX const boolexpr_Point_val(POINT);

void boolexpr_Soln_del(SOLN);
_Bool boolexpr_Soln_first(SOLN);
POINT boolexpr_Soln_second(SOLN);

SAT_ITER boolexpr_SatIter_new(BX);
void boolexpr_SatIter_del(SAT_ITER);
void boolexpr_SatIter_next(SAT_ITER);
POINT boolexpr_SatIter_val(SAT_ITER);

DFS_ITER boolexpr_DfsIter_new(BX);
void boolexpr_DfsIter_del(DFS_ITER);
void boolexpr_DfsIter_next(DFS_ITER);
BX boolexpr_DfsIter_val(DFS_ITER);

DOM_ITER boolexpr_DomainIter_new(BX);
void boolexpr_DomainIter_del(DOM_ITER);
void boolexpr_DomainIter_next(DOM_ITER);
POINT boolexpr_DomainIter_val(DOM_ITER);

CF_ITER boolexpr_CofactorIter_new(BX, size_t, VARS);
void boolexpr_CofactorIter_del(CF_ITER);
void boolexpr_CofactorIter_next(CF_ITER);
BX boolexpr_CofactorIter_val(CF_ITER);

BX boolexpr_zero(void);
BX boolexpr_one(void);
BX boolexpr_logical(void);
BX boolexpr_illogical(void);

BX boolexpr_not(BX);
BX boolexpr_nor(size_t, BXS);
BX boolexpr_or(size_t, BXS);
BX boolexpr_nand(size_t, BXS);
BX boolexpr_and(size_t, BXS);
BX boolexpr_xnor(size_t, BXS);
BX boolexpr_xor(size_t, BXS);
BX boolexpr_neq(size_t, BXS);
BX boolexpr_eq(size_t, BXS);
BX boolexpr_nimpl(BX, BX);
BX boolexpr_impl(BX, BX);
BX boolexpr_nite(BX, BX, BX);
BX boolexpr_ite(BX, BX, BX);

BX boolexpr_onehot0(size_t, BXS);
BX boolexpr_onehot(size_t, BXS);

BX boolexpr_nor_s(size_t, BXS);
BX boolexpr_or_s(size_t, BXS);
BX boolexpr_nand_s(size_t, BXS);
BX boolexpr_and_s(size_t, BXS);
BX boolexpr_xnor_s(size_t, BXS);
BX boolexpr_xor_s(size_t, BXS);
BX boolexpr_neq_s(size_t, BXS);
BX boolexpr_eq_s(size_t, BXS);
BX boolexpr_nimpl_s(BX, BX);
BX boolexpr_impl_s(BX, BX);
BX boolexpr_nite_s(BX, BX, BX);
BX boolexpr_ite_s(BX, BX, BX);

void boolexpr_BoolExpr_del(BX);
uint8_t boolexpr_BoolExpr_kind(BX);
STRING boolexpr_BoolExpr_to_string(BX);
uint32_t boolexpr_BoolExpr_depth(BX);
uint32_t boolexpr_BoolExpr_size(BX);
_Bool boolexpr_BoolExpr_is_cnf(BX);
_Bool boolexpr_BoolExpr_is_dnf(BX);
BX boolexpr_BoolExpr_simplify(BX);
BX boolexpr_BoolExpr_to_binop(BX);
BX boolexpr_BoolExpr_to_latop(BX);
BX boolexpr_BoolExpr_to_posop(BX);
BX boolexpr_BoolExpr_tseytin(BX, CONTEXT, STRING);
BX boolexpr_BoolExpr_compose(BX, size_t, VARS, BXS);
BX boolexpr_BoolExpr_restrict(BX, size_t, VARS, CONSTS);
BX boolexpr_BoolExpr_sat(BX);
BX boolexpr_BoolExpr_to_cnf(BX);
BX boolexpr_BoolExpr_to_dnf(BX);
BX boolexpr_BoolExpr_to_nnf(BX);
_Bool boolexpr_BoolExpr_equiv(BX, BX);
VARSET boolexpr_BoolExpr_support(BX);

BX boolexpr_BoolExpr_smoothing(BX, size_t, VARS);
BX boolexpr_BoolExpr_consensus(BX, size_t, VARS);
BX boolexpr_BoolExpr_derivative(BX, size_t, VARS);

CONTEXT boolexpr_Literal_ctx(BX);
uint32_t boolexpr_Literal_id(BX);

_Bool boolexpr_Operator_simple(BX);
VEC boolexpr_Operator_args(BX);
_Bool boolexpr_Operator_is_clause(BX);

"""

ffi = cffi.FFI()

ffi.set_source(
    "_boolexpr",
    HEADER,
    language="c++",
    define_macros=[],
    extra_compile_args=["-std=c++11"],
    include_dirs=INCLUDE_DIRS,
    sources=SOURCES,
)

ffi.cdef(HEADER)


if __name__ == "__main__":
    ffi.compile(verbose=True)
