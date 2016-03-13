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


CMSAT = join("third_party", "cryptominisat")


HEADER = """
    void * boolexpr_Context(void);
    void const * boolexpr_Context_get_var(void *, char const *);

    void const * boolexpr_zero(void);
    void const * boolexpr_one(void);

    void const * boolexpr_not(void const *);
    void const * boolexpr_or(uint32_t, void const **);
    void const * boolexpr_and(uint32_t, void const **);
    void const * boolexpr_xor(uint32_t, void const **);
    void const * boolexpr_eq(uint32_t, void const **);
    void const * boolexpr_impl(void const *, void const *);
    void const * boolexpr_ite(void const *, void const *, void const *);

    uint32_t boolexpr_BoolExpr_kind(void const *);
    char const * boolexpr_BoolExpr_to_string(void const *);
    uint32_t boolexpr_BoolExpr_depth(void const *);
"""

SOURCES = glob(join("src", "*.cc"))

INCLUDE_DIRS = [
    "include",
    join(CMSAT, "include"),
]

LIBRARY_DIRS = [
    join(CMSAT, "lib"),
]

LIBRARIES = [
    "cryptominisat4"
]

ffi = cffi.FFI()

ffi.set_source(
    "_boolexpr",
    HEADER,
    language="c++",
    sources=SOURCES,
    define_macros=[],
    extra_compile_args=["-std=c++11"],
    extra_link_args=[],
    include_dirs=INCLUDE_DIRS,
    library_dirs=LIBRARY_DIRS,
    libraries=LIBRARIES,
)

ffi.cdef(HEADER)


if __name__ == "__main__":
    ffi.compile(verbose=True)
