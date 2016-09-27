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


#ifndef BOOLEXPR_H_
#define BOOLEXPR_H_


#ifdef __cplusplus


#include <boost/optional.hpp>
#include "core/Solver.h"  // Solver, lbool, vec

#include <functional>  // function
#include <initializer_list>
#include <iterator>
#include <memory>  // enable_shared_from_this, shared_ptr, unique_ptr
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>  // pair
#include <vector>


// Kind checks
#define IS_ZERO(expr)  ((expr)->kind == boolexpr::BoolExpr::ZERO)
#define IS_ONE(expr)   ((expr)->kind == boolexpr::BoolExpr::ONE)
#define IS_LOG(expr)   ((expr)->kind == boolexpr::BoolExpr::LOG)
#define IS_ILL(expr)   ((expr)->kind == boolexpr::BoolExpr::ILL)
#define IS_COMP(expr)  ((expr)->kind == boolexpr::BoolExpr::COMP)
#define IS_VAR(expr)   ((expr)->kind == boolexpr::BoolExpr::VAR)
#define IS_NOR(expr)   ((expr)->kind == boolexpr::BoolExpr::NOR)
#define IS_OR(expr)    ((expr)->kind == boolexpr::BoolExpr::OR)
#define IS_NAND(expr)  ((expr)->kind == boolexpr::BoolExpr::NAND)
#define IS_AND(expr)   ((expr)->kind == boolexpr::BoolExpr::AND)
#define IS_XNOR(expr)  ((expr)->kind == boolexpr::BoolExpr::XNOR)
#define IS_XOR(expr)   ((expr)->kind == boolexpr::BoolExpr::XOR)
#define IS_NEQ(expr)   ((expr)->kind == boolexpr::BoolExpr::NEQ)
#define IS_EQ(expr)    ((expr)->kind == boolexpr::BoolExpr::EQ)
#define IS_NIMPL(expr) ((expr)->kind == boolexpr::BoolExpr::NIMPL)
#define IS_IMPL(expr)  ((expr)->kind == boolexpr::BoolExpr::IMPL)
#define IS_NITE(expr)  ((expr)->kind == boolexpr::BoolExpr::NITE)
#define IS_ITE(expr)   ((expr)->kind == boolexpr::BoolExpr::ITE)

#define ARE_SAME(ex1, ex2) ((ex1)->kind == (ex2)->kind)

// Category checks
#define IS_ATOM(expr)    ((expr)->kind >> 4 == 0) // 0 ****
#define IS_OP(expr)      ((expr)->kind >> 4 == 1) // 1 ****
#define IS_CONST(expr)   ((expr)->kind >> 3 == 0) // 0 0***
#define IS_KNOWN(expr)   ((expr)->kind >> 2 == 0) // 0 00**
#define IS_UNKNOWN(expr) ((expr)->kind >> 2 == 1) // 0 01**
#define IS_LIT(expr)     ((expr)->kind >> 3 == 1) // 0 1***
#define IS_NARY(expr)    ((expr)->kind >> 3 == 2) // 1 0***
#define IS_NEG(expr)     (!((expr)->kind & 1))    // * ***0
#define IS_POS(expr)     ((expr)->kind & 1)       // * ***1


/// Top-level namespace
namespace boolexpr {


// Forward declarations
class Context;
class BoolExpr;
class Constant;
class Zero;
class One;
class Logical;
class Illogical;
class Literal;
class Variable;
class Operator;
class LatticeOperator;
class Array;
class sat_iter;


using id_t = uint32_t;

using bx_t = std::shared_ptr<BoolExpr const>;

using const_t = std::shared_ptr<Constant const>;
using zero_t = std::shared_ptr<Zero const>;
using one_t = std::shared_ptr<One const>;
using log_t = std::shared_ptr<Logical const>;
using ill_t = std::shared_ptr<Illogical const>;

using lit_t = std::shared_ptr<Literal const>;
using var_t = std::shared_ptr<Variable const>;
using op_t = std::shared_ptr<Operator const>;
using lop_t = std::shared_ptr<LatticeOperator const>;

using var2bx_t = std::unordered_map<var_t, bx_t>;
using var2op_t = std::unordered_map<var_t, op_t>;
using point_t = std::unordered_map<var_t, const_t>;

using soln_t = std::pair<bool, boost::optional<point_t>>;

using array_t = std::unique_ptr<Array>;


class Context
{
    friend class Complement;
    friend class Variable;

public:
    Context();

    var_t get_var(std::string name);

private:
    id_t id;

    std::unordered_map<std::string, var_t> vars;
    std::unordered_map<id_t, std::string> id2name;
    std::unordered_map<id_t, lit_t> id2lit;

    std::string get_name(id_t id) const;
    lit_t get_lit(id_t id) const;
};


class BoolExpr : public std::enable_shared_from_this<BoolExpr>
{
    friend class Operator;
    friend class sat_iter;

    friend bx_t operator~(bx_t const &);
    friend std::ostream & operator<<(std::ostream &, bx_t const &);

public:
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

    Kind const kind;

    BoolExpr(Kind kind);

    std::string to_string() const;
    std::string to_dot() const;

    virtual uint32_t depth() const = 0;
    virtual uint32_t size() const = 0;

    virtual bool is_cnf() const = 0;
    virtual bool is_dnf() const = 0;

    virtual bx_t simplify() const = 0;
    virtual bx_t to_binop() const = 0;
    virtual bx_t to_cnf() const = 0;
    virtual bx_t to_dnf() const = 0;
    virtual bx_t to_latop() const = 0;
    virtual bx_t to_posop() const = 0;
    virtual bx_t tseytin(Context &, std::string const & = "a") const = 0;

    virtual bx_t compose(var2bx_t const &) const = 0;
    virtual bx_t restrict_(point_t const &) const = 0;

    soln_t sat() const;

    bx_t to_nnf() const;

    bool equiv(bx_t const &) const;
    std::unordered_set<var_t> support() const;
    uint32_t degree() const;

    bx_t expand(std::vector<var_t> const &) const;

    bx_t smoothing(std::vector<var_t> const &) const;
    bx_t consensus(std::vector<var_t> const &) const;
    bx_t derivative(std::vector<var_t> const &) const;

protected:
    virtual bx_t invert() const = 0;
    virtual std::ostream & op_lsh(std::ostream &) const = 0;
    virtual void dot_node(std::ostream &) const = 0;
    virtual void dot_edge(std::ostream &) const = 0;
    virtual soln_t _sat() const = 0;
    virtual void insert_support_var(std::unordered_set<var_t> &) const = 0;
    virtual bx_t find_subop(bool &, Context &, std::string const &, uint32_t &, var2op_t &) const = 0;
    virtual void sat_iter_init(sat_iter * const) const = 0;
};


class Atom : public BoolExpr
{
public:
    Atom(Kind kind);

    uint32_t depth() const;
    uint32_t size() const;
    bool is_cnf() const;
    bool is_dnf() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
    bx_t tseytin(Context &, std::string const & = "a") const;

protected:
    void dot_edge(std::ostream &) const;
    void insert_support_var(std::unordered_set<var_t> &) const;
    bx_t find_subop(bool &, Context &, std::string const &, uint32_t &, var2op_t &) const;
};


class Constant : public Atom
{
public:
    Constant(Kind kind);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
};


class Known : public Constant
{
public:
    bool const val;

    Known(Kind kind, bool val);
};


class Zero final : public Known
{
public:
    Zero();

    bool is_dnf() const;

protected:
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
    void sat_iter_init(sat_iter * const) const;
};


class One final : public Known
{
public:
    One();

    bool is_cnf() const;

protected:
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
    void sat_iter_init(sat_iter * const) const;
};


class Unknown : public Constant
{
public:
    Unknown(Kind kind);

protected:
    void sat_iter_init(sat_iter * const) const;
};


class Logical final : public Unknown
{
public:
    Logical();

protected:
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
};


class Illogical final : public Unknown
{
public:
    Illogical();

protected:
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
};


class Literal : public Atom
{
    friend lit_t abs(lit_t const &);

public:
    Context * const ctx;
    id_t const id;

    Literal(Kind kind, Context * const ctx, id_t id);

    bool is_cnf() const;
    bool is_dnf() const;

protected:
    virtual lit_t abs() const = 0;
};


class Complement final : public Literal
{
public:
    Complement(Context * const ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;

protected:
    lit_t abs() const;
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
    void insert_support_var(std::unordered_set<var_t> &) const;
    void sat_iter_init(sat_iter * const) const;
};


class Variable final : public Literal
{
public:
    Variable(Context * const ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;

protected:
    lit_t abs() const;
    bx_t invert() const;
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    soln_t _sat() const;
    void insert_support_var(std::unordered_set<var_t> &) const;
    void sat_iter_init(sat_iter * const) const;
};


class Operator : public BoolExpr
{
public:
    bool const simple;
    std::vector<bx_t> const args;

    Operator(Kind kind, bool simple, std::vector<bx_t> const & args);
    Operator(Kind kind, bool simple, std::vector<bx_t> const && args);

    uint32_t depth() const;
    uint32_t size() const;

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t simplify() const;
    bx_t tseytin(Context &, std::string const & = "a") const;
    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;

    bool is_clause() const;

protected:
    std::ostream & op_lsh(std::ostream &) const;
    void dot_node(std::ostream &) const;
    void dot_edge(std::ostream &) const;
    soln_t _sat() const;
    void insert_support_var(std::unordered_set<var_t> &) const;
    bx_t find_subop(bool &, Context &, std::string const &, uint32_t &, var2op_t &) const;
    void sat_iter_init(sat_iter * const) const;

    virtual std::string const opname_camel() const = 0;
    virtual std::string const opname_compact() const = 0;
    virtual bx_t _simplify() const = 0;
    virtual bx_t eqvar(var_t const &) const = 0;
    virtual op_t from_args(std::vector<bx_t> const &&) const = 0;

    op_t transform(std::function<bx_t(bx_t const &)>) const;

private:
    var_t to_con1(Context &, std::string const &, uint32_t &, var2op_t &) const;
    op_t  to_con2(Context &, std::string const &, uint32_t &, var2op_t &) const;
};


class NegativeOperator : public Operator
{
public:
    NegativeOperator(Kind kind, bool simple, std::vector<bx_t> const & args);

    bx_t to_binop() const;
    bx_t to_latop() const;

protected:
    bx_t _simplify() const;
};


class LatticeOperator : public Operator
{
public:
    LatticeOperator(Kind kind, bool simple, std::vector<bx_t> const & args);

    bx_t to_latop() const;
};


class Nor final : public NegativeOperator
{
public:
    Nor(bool simple, std::vector<bx_t> const & args);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Or final : public LatticeOperator
{
public:
    Or(bool simple, std::vector<bx_t> const & args);
    Or(bool simple, std::vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Nand final : public NegativeOperator
{
public:
    Nand(bool simple, std::vector<bx_t> const & args);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class And final : public LatticeOperator
{
public:
    And(bool simple, std::vector<bx_t> const & args);
    And(bool simple, std::vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Xnor final : public NegativeOperator
{
public:
    Xnor(bool simple, std::vector<bx_t> const & args);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Xor final : public Operator
{
public:
    Xor(bool simple, std::vector<bx_t> const & args);
    Xor(bool simple, std::vector<bx_t> const && args);

    static bx_t identity();

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(const std::vector<bx_t> &&) const;
};


class Unequal final : public NegativeOperator
{
public:
    Unequal(bool simple, std::vector<bx_t> const & args);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Equal final : public Operator
{
public:
    Equal(bool simple, std::vector<bx_t> const & args);
    Equal(bool simple, std::vector<bx_t> const && args);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class NotImplies final : public NegativeOperator
{
public:
    NotImplies(bool simple, bx_t p, bx_t q);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Implies final : public Operator
{
public:
    Implies(bool simple, bx_t p, bx_t q);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class NotIfThenElse final : public NegativeOperator
{
public:
    NotIfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class IfThenElse final : public Operator
{
public:
    IfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;

protected:
    bx_t invert() const;

    std::string const opname_camel() const;
    std::string const opname_compact() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(std::vector<bx_t> const &&) const;
};


class Array
{
    friend array_t operator~(Array const &);
    friend array_t operator|(Array const &, Array const &);
    friend array_t operator&(Array const &, Array const &);
    friend array_t operator^(Array const &, Array const &);

    friend array_t operator+(Array const &, Array const &);
    friend array_t operator*(Array const &, size_t);
    friend array_t operator*(size_t, Array const &);

public:
    Array();
    Array(std::vector<bx_t> const &);
    Array(std::vector<bx_t> const &&);
    Array(std::initializer_list<bx_t> const items);

    bx_t const & operator[](size_t) const;
    bx_t & operator[](size_t);

    size_t size() const;
    std::vector<bx_t>::const_iterator begin() const;
    std::vector<bx_t>::const_iterator end() const;

    array_t simplify() const;
    array_t compose(var2bx_t const &) const;
    array_t restrict_(point_t const &) const;
    bool equiv(Array const &) const;

    array_t zext(size_t num) const;
    array_t sext(size_t num) const;

    bx_t nor_reduce() const;
    bx_t or_reduce() const;
    bx_t nand_reduce() const;
    bx_t and_reduce() const;
    bx_t xnor_reduce() const;
    bx_t xor_reduce() const;

    std::pair<array_t, array_t> lsh(Array const &) const;
    std::pair<array_t, array_t> rsh(Array const &) const;
    std::pair<array_t, array_t> arsh(size_t) const;

private:
    std::vector<bx_t> items;
};


class dfs_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
public:
    dfs_iter();
    dfs_iter(bx_t const &);

    bool operator==(dfs_iter const &) const;
    bool operator!=(dfs_iter const &) const;
    bx_t const & operator*() const;
    dfs_iter const & operator++();

private:
    enum class Color { WHITE, GRAY, BLACK };

    std::unordered_map<bx_t, Color> colors;
    std::vector<bx_t> stack;

    bx_t const * p;

    void advance_one();
};


class sat_iter : public std::iterator<std::input_iterator_tag, point_t>
{
    friend class Zero;
    friend class One;
    friend class Unknown;
    friend class Complement;
    friend class Variable;
    friend class Operator;

public:
    sat_iter();
    sat_iter(bx_t const &);

    bool operator==(sat_iter const &) const;
    bool operator!=(sat_iter const &) const;
    point_t const & operator*() const;
    sat_iter const & operator++();

private:
    Context ctx;
    std::unordered_map<uint32_t, var_t> idx2var;

    Glucose::Solver solver;

    bool sat;
    point_t point;

    bool one_soln;

    void get_soln();
};


class space_iter : public std::iterator<std::input_iterator_tag, std::vector<bool>>
{
public:
    space_iter();
    space_iter(size_t n);

    bool operator==(space_iter const &) const;
    bool operator!=(space_iter const &) const;
    std::vector<bool> const & operator*() const;
    space_iter const & operator++();
    bool parity() const;

private:
    size_t n;
    std::vector<bool> counter;
};


class points_iter : public std::iterator<std::input_iterator_tag, point_t>
{
public:
    points_iter();
    points_iter(std::vector<var_t> const &);

    bool operator==(points_iter const &) const;
    bool operator!=(points_iter const &) const;
    point_t const & operator*() const;
    points_iter const & operator++();

private:
    space_iter it;
    std::vector<var_t> vars;

    point_t point;
};


class terms_iter : public std::iterator<std::input_iterator_tag, std::vector<bx_t>>
{
public:
    terms_iter();
    terms_iter(std::vector<var_t> const &);
    terms_iter(std::vector<bx_t> const &);

    bool operator==(terms_iter const &) const;
    bool operator!=(terms_iter const &) const;
    std::vector<bx_t> const & operator*() const;
    terms_iter const & operator++();

private:
    space_iter it;
    std::vector<bx_t> bxs;

    std::vector<bx_t> term;
};


class domain_iter : public std::iterator<std::input_iterator_tag, point_t>
{
public:
    domain_iter();
    domain_iter(bx_t const &);

    bool operator==(domain_iter const &) const;
    bool operator!=(domain_iter const &) const;
    point_t const & operator*() const;
    domain_iter const & operator++();

private:
    std::unordered_set<var_t> s;
    points_iter it;
};


class cf_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
public:
    cf_iter();
    cf_iter(bx_t const &, std::vector<var_t> const &);

    bool operator==(cf_iter const &) const;
    bool operator!=(cf_iter const &) const;
    bx_t const & operator*() const;
    cf_iter const & operator++();

private:
    bx_t f;
    points_iter it;
    bx_t cf;
};


/// Return Boolean zero.
zero_t zero();

/// Return Boolean one.
one_t one();

/// Return Boolean "logical".
log_t logical();

ill_t illogical();

bx_t nor(std::vector<bx_t> const &);
bx_t nor(std::vector<bx_t> const &&);
bx_t nor(std::initializer_list<bx_t> const);
bx_t or_(std::vector<bx_t> const &);
bx_t or_(std::vector<bx_t> const &&);
bx_t or_(std::initializer_list<bx_t> const);
bx_t nand(std::vector<bx_t> const &);
bx_t nand(std::vector<bx_t> const &&);
bx_t nand(std::initializer_list<bx_t> const);
bx_t and_(std::vector<bx_t> const &);
bx_t and_(std::vector<bx_t> const &&);
bx_t and_(std::initializer_list<bx_t> const);
bx_t xnor(std::vector<bx_t> const &);
bx_t xnor(std::vector<bx_t> const &&);
bx_t xnor(std::initializer_list<bx_t> const);
bx_t xor_(std::vector<bx_t> const &);
bx_t xor_(std::vector<bx_t> const &&);
bx_t xor_(std::initializer_list<bx_t> const);
bx_t neq(std::vector<bx_t> const &);
bx_t neq(std::vector<bx_t> const &&);
bx_t neq(std::initializer_list<bx_t> const);
bx_t eq(std::vector<bx_t> const &);
bx_t eq(std::vector<bx_t> const &&);
bx_t eq(std::initializer_list<bx_t> const);
bx_t nimpl(bx_t const &, bx_t const &);
bx_t impl(bx_t const &, bx_t const &);
bx_t nite(bx_t const &, bx_t const &, bx_t const &);
bx_t ite(bx_t const &, bx_t const &, bx_t const &);

bx_t onehot0(std::vector<bx_t> const &);
bx_t onehot0(std::vector<bx_t> const &&);
bx_t onehot0(std::initializer_list<bx_t> const);
bx_t onehot(std::vector<bx_t> const &);
bx_t onehot(std::vector<bx_t> const &&);
bx_t onehot(std::initializer_list<bx_t> const);

bx_t nor_s(std::vector<bx_t> const &);
bx_t nor_s(std::vector<bx_t> const &&);
bx_t nor_s(std::initializer_list<bx_t> const);
bx_t or_s(std::vector<bx_t> const &);
bx_t or_s(std::vector<bx_t> const &&);
bx_t or_s(std::initializer_list<bx_t> const);
bx_t nand_s(std::vector<bx_t> const &);
bx_t nand_s(std::vector<bx_t> const &&);
bx_t nand_s(std::initializer_list<bx_t> const);
bx_t and_s(std::vector<bx_t> const &);
bx_t and_s(std::vector<bx_t> const &&);
bx_t and_s(std::initializer_list<bx_t> const);
bx_t xnor_s(std::vector<bx_t> const &);
bx_t xnor_s(std::vector<bx_t> const &&);
bx_t xnor_s(std::initializer_list<bx_t> const);
bx_t xor_s(std::vector<bx_t> const &);
bx_t xor_s(std::vector<bx_t> const &&);
bx_t xor_s(std::initializer_list<bx_t> const);
bx_t neq_s(std::vector<bx_t> const &);
bx_t neq_s(std::vector<bx_t> const &&);
bx_t neq_s(std::initializer_list<bx_t> const);
bx_t eq_s(std::vector<bx_t> const &);
bx_t eq_s(std::vector<bx_t> const &&);
bx_t eq_s(std::initializer_list<bx_t> const);
bx_t nimpl_s(bx_t const &, bx_t const &);
bx_t impl_s(bx_t const &, bx_t const &);
bx_t nite_s(bx_t const &, bx_t const &, bx_t const &);
bx_t ite_s(bx_t const &, bx_t const &, bx_t const &);

bx_t operator~(bx_t const &);
bx_t operator|(bx_t const &, bx_t const &);
bx_t operator&(bx_t const &, bx_t const &);
bx_t operator^(bx_t const &, bx_t const &);
lit_t abs(lit_t const &);
bool operator<(lit_t const &, lit_t const &);
std::ostream & operator<<(std::ostream &, bx_t const &);

array_t operator~(Array const &);
array_t operator|(Array const &, Array const &);
array_t operator&(Array const &, Array const &);
array_t operator^(Array const &, Array const &);
array_t operator+(Array const &, Array const &);
array_t operator*(Array const &, size_t);
array_t operator*(size_t, Array const &);


} // namespace boolexpr


#endif  // __cplusplus


#ifdef __GNUC__
#    define DllExport
#elif __clang__
#    define DllExport
#elif _MSC_VER
#    define DllExport __declspec(dllexport)
#endif


// C Foreign Function Interface (CFFI)
extern "C" {


typedef char const * const STRING;
typedef void * const CONTEXT;
typedef void const * const BX;
typedef void const * const LIT;
typedef void * const ARRAY;
typedef void * const ARRAY_PAIR;
typedef void const * const * const BXS;
typedef void const * const * const VARS;
typedef void const * const * const CONSTS;
typedef void * const VEC;
typedef void * const VARSET;
typedef void * const POINT;
typedef void * const SOLN;
typedef void * const DFS_ITER;
typedef void * const SAT_ITER;
typedef void * const POINTS_ITER;
typedef void * const TERMS_ITER;
typedef void * const DOM_ITER;
typedef void * const CF_ITER;


DllExport CONTEXT boolexpr_Context_new(void);
DllExport void boolexpr_Context_del(CONTEXT);
DllExport BX boolexpr_Context_get_var(CONTEXT, STRING);

DllExport void boolexpr_String_del(STRING);

DllExport void boolexpr_Vec_del(VEC);
DllExport void boolexpr_Vec_iter(VEC);
DllExport void boolexpr_Vec_next(VEC);
DllExport BX boolexpr_Vec_val(VEC);

DllExport void boolexpr_VarSet_del(VARSET);
DllExport void boolexpr_VarSet_iter(VARSET);
DllExport void boolexpr_VarSet_next(VARSET);
DllExport BX boolexpr_VarSet_val(VARSET);

DllExport void boolexpr_Point_del(POINT);
DllExport void boolexpr_Point_iter(POINT);
DllExport void boolexpr_Point_next(POINT);
DllExport BX boolexpr_Point_key(POINT);
DllExport BX boolexpr_Point_val(POINT);

DllExport void boolexpr_Soln_del(SOLN);
DllExport bool boolexpr_Soln_first(SOLN);
DllExport POINT boolexpr_Soln_second(SOLN);

DllExport DFS_ITER boolexpr_DfsIter_new(BX);
DllExport void boolexpr_DfsIter_del(DFS_ITER);
DllExport void boolexpr_DfsIter_next(DFS_ITER);
DllExport BX boolexpr_DfsIter_val(DFS_ITER);

DllExport SAT_ITER boolexpr_SatIter_new(BX);
DllExport void boolexpr_SatIter_del(SAT_ITER);
DllExport void boolexpr_SatIter_next(SAT_ITER);
DllExport POINT boolexpr_SatIter_val(SAT_ITER);

DllExport POINTS_ITER boolexpr_PointsIter_new(size_t, VARS);
DllExport void boolexpr_PointsIter_del(POINTS_ITER);
DllExport void boolexpr_PointsIter_next(POINTS_ITER);
DllExport POINT boolexpr_PointsIter_val(POINTS_ITER);

DllExport TERMS_ITER boolexpr_TermsIter_new(size_t, VARS);
DllExport void boolexpr_TermsIter_del(TERMS_ITER);
DllExport void boolexpr_TermsIter_next(TERMS_ITER);
DllExport VEC boolexpr_TermsIter_val(TERMS_ITER);

DllExport DOM_ITER boolexpr_DomainIter_new(BX);
DllExport void boolexpr_DomainIter_del(DOM_ITER);
DllExport void boolexpr_DomainIter_next(DOM_ITER);
DllExport POINT boolexpr_DomainIter_val(DOM_ITER);

DllExport CF_ITER boolexpr_CofactorIter_new(BX, size_t, VARS);
DllExport void boolexpr_CofactorIter_del(CF_ITER);
DllExport void boolexpr_CofactorIter_next(CF_ITER);
DllExport BX boolexpr_CofactorIter_val(CF_ITER);

DllExport BX boolexpr_zero(void);
DllExport BX boolexpr_one(void);
DllExport BX boolexpr_logical(void);
DllExport BX boolexpr_illogical(void);

DllExport BX boolexpr_not(BX);
DllExport LIT boolexpr_abs(LIT);
DllExport BX boolexpr_nor(size_t, BXS);
DllExport BX boolexpr_or(size_t, BXS);
DllExport BX boolexpr_nand(size_t, BXS);
DllExport BX boolexpr_and(size_t, BXS);
DllExport BX boolexpr_xnor(size_t, BXS);
DllExport BX boolexpr_xor(size_t, BXS);
DllExport BX boolexpr_neq(size_t, BXS);
DllExport BX boolexpr_eq(size_t, BXS);
DllExport BX boolexpr_nimpl(BX, BX);
DllExport BX boolexpr_impl(BX, BX);
DllExport BX boolexpr_nite(BX, BX, BX);
DllExport BX boolexpr_ite(BX, BX, BX);
DllExport BX boolexpr_onehot0(size_t, BXS);
DllExport BX boolexpr_onehot(size_t, BXS);

DllExport BX boolexpr_nor_s(size_t, BXS);
DllExport BX boolexpr_or_s(size_t, BXS);
DllExport BX boolexpr_nand_s(size_t, BXS);
DllExport BX boolexpr_and_s(size_t, BXS);
DllExport BX boolexpr_xnor_s(size_t, BXS);
DllExport BX boolexpr_xor_s(size_t, BXS);
DllExport BX boolexpr_neq_s(size_t, BXS);
DllExport BX boolexpr_eq_s(size_t, BXS);
DllExport BX boolexpr_nimpl_s(BX, BX);
DllExport BX boolexpr_impl_s(BX, BX);
DllExport BX boolexpr_nite_s(BX, BX, BX);
DllExport BX boolexpr_ite_s(BX, BX, BX);

DllExport void boolexpr_BoolExpr_del(BX);
DllExport uint8_t boolexpr_BoolExpr_kind(BX);
DllExport STRING boolexpr_BoolExpr_to_string(BX);
DllExport STRING boolexpr_BoolExpr_to_dot(BX);
DllExport uint32_t boolexpr_BoolExpr_depth(BX);
DllExport uint32_t boolexpr_BoolExpr_size(BX);
DllExport bool boolexpr_BoolExpr_is_cnf(BX);
DllExport bool boolexpr_BoolExpr_is_dnf(BX);
DllExport BX boolexpr_BoolExpr_simplify(BX);
DllExport BX boolexpr_BoolExpr_to_binop(BX);
DllExport BX boolexpr_BoolExpr_to_latop(BX);
DllExport BX boolexpr_BoolExpr_to_posop(BX);
DllExport BX boolexpr_BoolExpr_tseytin(BX, CONTEXT, STRING);
DllExport BX boolexpr_BoolExpr_compose(BX, size_t, VARS, BXS);
DllExport BX boolexpr_BoolExpr_restrict(BX, size_t, VARS, CONSTS);
DllExport BX boolexpr_BoolExpr_sat(BX);
DllExport BX boolexpr_BoolExpr_to_cnf(BX);
DllExport BX boolexpr_BoolExpr_to_dnf(BX);
DllExport BX boolexpr_BoolExpr_to_nnf(BX);
DllExport bool boolexpr_BoolExpr_equiv(BX, BX);
DllExport VARSET boolexpr_BoolExpr_support(BX);
DllExport uint32_t boolexpr_BoolExpr_degree(BX);

DllExport BX boolexpr_BoolExpr_expand(BX, size_t, VARS);

DllExport BX boolexpr_BoolExpr_smoothing(BX, size_t, VARS);
DllExport BX boolexpr_BoolExpr_consensus(BX, size_t, VARS);
DllExport BX boolexpr_BoolExpr_derivative(BX, size_t, VARS);

DllExport CONTEXT boolexpr_Literal_ctx(BX);
DllExport uint32_t boolexpr_Literal_id(BX);

DllExport bool boolexpr_Operator_simple(BX);
DllExport VEC boolexpr_Operator_args(BX);
DllExport bool boolexpr_Operator_is_clause(BX);

DllExport ARRAY boolexpr_Array_new(size_t, BXS);
DllExport void boolexpr_Array_del(ARRAY);
DllExport size_t boolexpr_Array_size(ARRAY);
DllExport BX boolexpr_Array_getitem(ARRAY, size_t);
DllExport void boolexpr_Array_setitem(ARRAY, size_t, BX);
DllExport ARRAY boolexpr_Array_getslice(ARRAY, size_t, size_t);
DllExport ARRAY boolexpr_Array_invert(ARRAY);
DllExport ARRAY boolexpr_Array_or(ARRAY, ARRAY);
DllExport ARRAY boolexpr_Array_and(ARRAY, ARRAY);
DllExport ARRAY boolexpr_Array_xor(ARRAY, ARRAY);
DllExport ARRAY boolexpr_Array_plus(ARRAY, ARRAY);
DllExport ARRAY boolexpr_Array_mul(ARRAY, size_t);
DllExport ARRAY boolexpr_Array_simplify(ARRAY);
DllExport ARRAY boolexpr_Array_compose(ARRAY, size_t, VARS, BXS);
DllExport ARRAY boolexpr_Array_restrict(ARRAY, size_t, VARS, CONSTS);
DllExport bool boolexpr_Array_equiv(ARRAY, ARRAY);
DllExport ARRAY boolexpr_Array_zext(ARRAY, size_t);
DllExport ARRAY boolexpr_Array_sext(ARRAY, size_t);
DllExport BX boolexpr_Array_nor_reduce(ARRAY);
DllExport BX boolexpr_Array_or_reduce(ARRAY);
DllExport BX boolexpr_Array_nand_reduce(ARRAY);
DllExport BX boolexpr_Array_and_reduce(ARRAY);
DllExport BX boolexpr_Array_xnor_reduce(ARRAY);
DllExport BX boolexpr_Array_xor_reduce(ARRAY);
DllExport ARRAY boolexpr_ArrayPair_fst(ARRAY_PAIR);
DllExport ARRAY boolexpr_ArrayPair_snd(ARRAY_PAIR);
DllExport void boolexpr_ArrayPair_del(ARRAY_PAIR);
DllExport ARRAY_PAIR boolexpr_Array_lsh(ARRAY, ARRAY);
DllExport ARRAY_PAIR boolexpr_Array_rsh(ARRAY, ARRAY);
DllExport ARRAY_PAIR boolexpr_Array_arsh(ARRAY, size_t);


} // extern "C"


#endif  // BOOLEXPR_H_
