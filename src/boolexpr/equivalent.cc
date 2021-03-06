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

namespace boolexpr {

bool BoolExpr::equiv(bx_t const& other) const {
    auto self = shared_from_this();
    auto soln = (self ^ other)->sat();
    return !soln.first;
}

}  // namespace boolexpr
