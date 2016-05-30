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


// WARNING:
//     The contents of this file are implementation details.
//     Do not use these declarations for anything,
//     because they may change without notice.


namespace boolexpr {


class ArgSet
{
protected:
    std::unordered_set<bx_t> args;
    virtual void insert(bx_t const &) = 0;
    virtual bx_t to_op() const = 0;

public:
    virtual bx_t reduce() const = 0;
};


class LatticeArgSet : public ArgSet
{
protected:
    enum class State { infimum, basic, islog, supremum, isill };
    State state;

    BoolExpr::Kind kind;
    bx_t identity;
    bx_t dominator;

    void insert(bx_t const &);

public:
    LatticeArgSet(vector<bx_t> const & args, BoolExpr::Kind const & kind,
                  bx_t const & identity, bx_t const & dominator);
    bx_t reduce() const;
};


class OrArgSet : public LatticeArgSet
{
protected:
    bx_t to_op() const;

public:
    OrArgSet(vector<bx_t> const & args);
};


class AndArgSet : public LatticeArgSet
{
protected:
    bx_t to_op() const;

public:
    AndArgSet(vector<bx_t> const & args);
};


class XorArgSet : public ArgSet
{
    enum class State { basic, islog, isill };
    State state;
    bool parity;

protected:
    void insert(bx_t const &);
    bx_t to_op() const;

public:
    XorArgSet(vector<bx_t> const & args);
    bx_t reduce() const;
};


class EqArgSet : public ArgSet
{
    enum class State { basic, islog, isill };
    State state;
    bool has_zero;
    bool has_one;

protected:
    void insert(bx_t const &);
    bx_t to_op() const;

public:
    EqArgSet(vector<bx_t> const & args);
    bx_t reduce() const;
};


} // namespace boolexpr
