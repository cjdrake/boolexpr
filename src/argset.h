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


#ifndef BOOLEXPR_ARGSET_H_
#define BOOLEXPR_ARGSET_H_


namespace boolexpr {


class ArgSet
{
public:
    virtual bx_t reduce() const = 0;

protected:
    std::unordered_set<bx_t> args;
    virtual void insert(bx_t const &) = 0;
    virtual bx_t to_op() const = 0;
};


class LatticeArgSet : public ArgSet
{
public:
    LatticeArgSet(std::vector<bx_t> const & args, BoolExpr::Kind const & kind,
                  bx_t const & identity, bx_t const & dominator);
    bx_t reduce() const;

protected:
    enum class State { infimum, basic, islog, supremum, isill };
    State state;

    BoolExpr::Kind kind;
    bx_t identity;
    bx_t dominator;

    void insert(bx_t const &);
};


class OrArgSet : public LatticeArgSet
{
public:
    OrArgSet(std::vector<bx_t> const & args);

protected:
    bx_t to_op() const;
};


class AndArgSet : public LatticeArgSet
{
public:
    AndArgSet(std::vector<bx_t> const & args);

protected:
    bx_t to_op() const;
};


class XorArgSet : public ArgSet
{
public:
    XorArgSet(std::vector<bx_t> const & args);
    bx_t reduce() const;

protected:
    void insert(bx_t const &);
    bx_t to_op() const;

private:
    enum class State { basic, islog, isill };
    State state;
    bool parity;
};


class EqArgSet : public ArgSet
{
public:
    EqArgSet(std::vector<bx_t> const & args);
    bx_t reduce() const;

protected:
    void insert(bx_t const &);
    bx_t to_op() const;

private:
    enum class State { basic, islog, isill };
    State state;
    bool has_zero;
    bool has_one;
};


} // namespace boolexpr


#endif  // BOOLEXPR_ARGSET_H_
