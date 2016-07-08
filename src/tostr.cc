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


#include <sstream>

#include "boolexpr/boolexpr.h"


using std::static_pointer_cast;
using std::string;


namespace boolexpr {


string const
Nor::opname() const
{
    return "Nor";
}


string const
Or::opname() const
{
    return "Or";
}


string const
Nand::opname() const
{
    return "Nand";
}


string const
And::opname() const
{
    return "And";
}


string const
Xnor::opname() const
{
    return "Xnor";
}


string const
Xor::opname() const
{
    return "Xor";
}


string const
Unequal::opname() const
{
    return "Unequal";
}


string const
Equal::opname() const
{
    return "Equal";
}


string const
NotImplies::opname() const
{
    return "NotImplies";
}


string const
Implies::opname() const
{
    return "Implies";
}


string const
NotIfThenElse::opname() const
{
    return "NotIfThenElse";
}


string const
IfThenElse::opname() const
{
    return "IfThenElse";
}


std::ostream&
Zero::op_lsh(std::ostream& s) const
{
    return s << "0";
}


std::ostream&
One::op_lsh(std::ostream& s) const
{
    return s << "1";
}


std::ostream&
Logical::op_lsh(std::ostream& s) const
{
    return s << "X";
}


std::ostream&
Illogical::op_lsh(std::ostream& s) const
{
    return s << "?";
}


std::ostream&
Complement::op_lsh(std::ostream& s) const
{
    auto self = shared_from_this();
    auto xn = static_pointer_cast<Complement const>(self);
    return s << "~" << xn->ctx->get_name(xn->id);
}


std::ostream&
Variable::op_lsh(std::ostream& s) const
{
    auto self = shared_from_this();
    auto x = static_pointer_cast<Variable const>(self);
    return s << x->ctx->get_name(x->id);
}


std::ostream&
Operator::op_lsh(std::ostream& s) const
{
    s << opname() << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0) {
            s << ", ";
        }
        s << args[i];
    }
    return s << ")";
}


std::ostream&
operator<<(std::ostream& s, bx_t const & bx)
{
    return bx->op_lsh(s);
}


string
BoolExpr::to_string() const
{
    std::ostringstream oss;
    oss << shared_from_this();
    return oss.str();
}


}  // namespace boolexpr
