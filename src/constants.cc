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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>
#include <cryptominisat4/cryptominisat.h>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


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
