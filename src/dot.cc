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


void
Zero::dot_node(std::ostream & s) const
{
    s << " n" << this << " [label=\"0\",shape=box];";
}


void
One::dot_node(std::ostream & s) const
{
    s << " n" << this << " [label=\"1\",shape=box];";
}


void
Logical::dot_node(std::ostream & s) const
{
    s << " n" << this << " [label=\"X\",shape=box];";
}


void
Illogical::dot_node(std::ostream & s) const
{
    s << " n" << this << " [label=\"?\",shape=box];";
}


void
Complement::dot_node(std::ostream & s) const
{
    auto xn = static_pointer_cast<Complement const>(shared_from_this());

    s << " n" << this;
    s << " [label=";
    s << "\"~" << xn->ctx->get_name(xn->id) << "\"";
    s << ",shape=box];";
}


void
Variable::dot_node(std::ostream & s) const
{
    auto x = static_pointer_cast<Variable const>(shared_from_this());

    s << " n" << this;
    s << " [label=";
    s << "\"" << x->ctx->get_name(x->id) << "\"";
    s << ",shape=box];";
}


void
Operator::dot_node(std::ostream & s) const
{
    s << " n" << this;
    s << " [label=";
    s << "\"" << opname_compact() << "\"";
    s << ",shape=circle];";
}


void
Atom::dot_edge(std::ostream & s) const
{}


void
Operator::dot_edge(std::ostream & s) const
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
