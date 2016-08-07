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
Nor::opname_camel() const
{
    return "Nor";
}


string const
Or::opname_camel() const
{
    return "Or";
}


string const
Nand::opname_camel() const
{
    return "Nand";
}


string const
And::opname_camel() const
{
    return "And";
}


string const
Xnor::opname_camel() const
{
    return "Xnor";
}


string const
Xor::opname_camel() const
{
    return "Xor";
}


string const
Unequal::opname_camel() const
{
    return "Unequal";
}


string const
Equal::opname_camel() const
{
    return "Equal";
}


string const
NotImplies::opname_camel() const
{
    return "NotImplies";
}


string const
Implies::opname_camel() const
{
    return "Implies";
}


string const
NotIfThenElse::opname_camel() const
{
    return "NotIfThenElse";
}


string const
IfThenElse::opname_camel() const
{
    return "IfThenElse";
}


string const
Nor::opname_compact() const
{
    return "~or";
}


string const
Or::opname_compact() const
{
    return "or";
}


string const
Nand::opname_compact() const
{
    return "~and";
}


string const
And::opname_compact() const
{
    return "and";
}


string const
Xnor::opname_compact() const
{
    return "~xor";
}


string const
Xor::opname_compact() const
{
    return "xor";
}


string const
Unequal::opname_compact() const
{
    return "~eq";
}


string const
Equal::opname_compact() const
{
    return "eq";
}


string const
NotImplies::opname_compact() const
{
    return "~impl";
}


string const
Implies::opname_compact() const
{
    return "impl";
}


string const
NotIfThenElse::opname_compact() const
{
    return "~ite";
}


string const
IfThenElse::opname_compact() const
{
    return "ite";
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
    s << opname_camel() << "(" << args[0];
    for (size_t i = 1; i < args.size(); ++i) {
        s << ", " << args[i];
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


void
Zero::dot_node(std::ostream& s) const
{
    s << " n" << this << " [label=\"0\",shape=box];";
}


void
One::dot_node(std::ostream& s) const
{
    s << " n" << this << " [label=\"1\",shape=box];";
}


void
Logical::dot_node(std::ostream& s) const
{
    s << " n" << this << " [label=\"X\",shape=box];";
}


void
Illogical::dot_node(std::ostream& s) const
{
    s << " n" << this << " [label=\"?\",shape=box];";
}


void
Complement::dot_node(std::ostream& s) const
{
    auto xn = static_pointer_cast<Complement const>(shared_from_this());

    s << " n" << this;
    s << " [label=";
    s << "\"~" << xn->ctx->get_name(xn->id) << "\"";
    s << ",shape=box];";
}


void
Variable::dot_node(std::ostream& s) const
{
    auto x = static_pointer_cast<Variable const>(shared_from_this());

    s << " n" << this;
    s << " [label=";
    s << "\"" << x->ctx->get_name(x->id) << "\"";
    s << ",shape=box];";
}


void
Operator::dot_node(std::ostream& s) const
{
    s << " n" << this;
    s << " [label=";
    s << "\"" << opname_compact() << "\"";
    s << ",shape=circle];";
}


void
Atom::dot_edge(std::ostream& s) const
{}


void
Operator::dot_edge(std::ostream& s) const
{
    for (size_t i = 0; i < args.size(); ++i) {
        s << " n" << &*args[i];
        s << " --";
        s << " n" << this;
        s << ";";
    }
}


string
BoolExpr::to_dot() const
{
    auto self = shared_from_this();

    std::ostringstream oss;

    oss << "graph {";
    oss << " rankdir=BT;";

    for (auto it = dfs_iter(self); it != dfs_iter(); ++it) {
        (*it)->dot_node(oss);
    }

    for (auto it = dfs_iter(self); it != dfs_iter(); ++it) {
        (*it)->dot_edge(oss);
    }

    oss << " }";

    return oss.str();
}


}  // namespace boolexpr
