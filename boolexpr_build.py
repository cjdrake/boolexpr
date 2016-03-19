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

void * boolexpr_Context_new(void);
void boolexpr_Context_del(void *);
void const * boolexpr_Context_get_var(void *, char const *);

void const * boolexpr_zero(void);
void const * boolexpr_one(void);
void const * boolexpr_logical(void);
void const * boolexpr_illogical(void);

void const * boolexpr_not(void const *);
void const * boolexpr_nor(uint32_t, void const **);
void const * boolexpr_or(uint32_t, void const **);
void const * boolexpr_nand(uint32_t, void const **);
void const * boolexpr_and(uint32_t, void const **);
void const * boolexpr_xnor(uint32_t, void const **);
void const * boolexpr_xor(uint32_t, void const **);
void const * boolexpr_neq(uint32_t, void const **);
void const * boolexpr_eq(uint32_t, void const **);
void const * boolexpr_impl(void const *, void const *);
void const * boolexpr_ite(void const *, void const *, void const *);
void const * boolexpr_nor_s(uint32_t, void const **);
void const * boolexpr_or_s(uint32_t, void const **);
void const * boolexpr_nand_s(uint32_t, void const **);
void const * boolexpr_and_s(uint32_t, void const **);
void const * boolexpr_xnor_s(uint32_t, void const **);
void const * boolexpr_xor_s(uint32_t, void const **);
void const * boolexpr_neq_s(uint32_t, void const **);
void const * boolexpr_eq_s(uint32_t, void const **);
void const * boolexpr_impl_s(void const *, void const *);
void const * boolexpr_ite_s(void const *, void const *, void const *);

void boolexpr_BoolExpr_del(void const *);
uint32_t boolexpr_BoolExpr_kind(void const *);
char const * boolexpr_BoolExpr_to_string(void const *);
void boolexpr_string_del(char const *);
uint32_t boolexpr_BoolExpr_depth(void const *);
uint32_t boolexpr_BoolExpr_size(void const *);
uint32_t boolexpr_BoolExpr_atom_count(void const *);
uint32_t boolexpr_BoolExpr_op_count(void const *);
_Bool boolexpr_BoolExpr_is_cnf(void const *);
_Bool boolexpr_BoolExpr_is_dnf(void const *);
void const * boolexpr_BoolExpr_pushdown_not(void const *);
void const * boolexpr_BoolExpr_simplify(void const *);
void const * boolexpr_BoolExpr_to_binop(void const *);
void const * boolexpr_BoolExpr_to_latop(void const *);
void const * boolexpr_BoolExpr_tseytin(void const *, void *, char const *);
void const * boolexpr_BoolExpr_to_cnf(void const *);
void const * boolexpr_BoolExpr_to_dnf(void const *);
void const * boolexpr_BoolExpr_to_nnf(void const *);
_Bool boolexpr_BoolExpr_equiv(void const *, void const *);

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
