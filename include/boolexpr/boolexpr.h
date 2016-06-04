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


#ifndef BOOLEXPR_H
#define BOOLEXPR_H


#ifdef __cplusplus


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


using std::string;
using std::vector;


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


class Context
{
    friend class Complement;
    friend class Variable;

    id_t id;

    std::unordered_map<string, var_t> vars;
    std::unordered_map<id_t, string> id2name;
    std::unordered_map<id_t, lit_t> id2lit;

    string get_name(id_t id) const;
    lit_t get_lit(id_t id) const;

public:
    Context();

    var_t get_var(string name);
};


class BoolExpr : public std::enable_shared_from_this<BoolExpr>
{
    friend bx_t operator~(bx_t const &);
    friend std::ostream& operator<<(std::ostream&, bx_t const &);

protected:
    virtual bx_t invert() const = 0;
    virtual std::ostream& op_lsh(std::ostream&) const = 0;
    virtual soln_t _sat() const = 0;

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

    string to_string() const;

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
    virtual bx_t tseytin(Context&, string const & = "a") const = 0;

    virtual bx_t compose(var2bx_t const &) const = 0;
    virtual bx_t restrict_(point_t const &) const = 0;

    soln_t sat() const;

    bx_t to_nnf() const;

    bool equiv(bx_t const &) const;
    std::unordered_set<var_t> support() const;

    bx_t smoothing(vector<var_t> const &) const;
    bx_t consensus(vector<var_t> const &) const;
    bx_t derivative(vector<var_t> const &) const;
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
    bx_t tseytin(Context&, string const & = "a") const;
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


class Zero : public Known
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    Zero();

    bool is_dnf() const;
};


class One : public Known
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    One();

    bool is_cnf() const;
};


class Unknown : public Constant
{
public:
    Unknown(Kind kind);
};


class Logical : public Unknown
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    Logical();
};


class Illogical : public Unknown
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    Illogical();
};


class Literal : public Atom
{
public:
    Context * const ctx;
    id_t const id;

    Literal(Kind kind, Context * const ctx, id_t id);

    bool is_cnf() const;
    bool is_dnf() const;
};


class Complement : public Literal
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    Complement(Context * const ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
};


class Variable : public Literal
{
protected:
    bx_t invert() const;
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

public:
    Variable(Context * const ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
};


class Operator : public BoolExpr
{
    var_t to_con1(Context&, string const &, uint32_t&, var2op_t&) const;
    op_t  to_con2(Context&, string const &, uint32_t&, var2op_t&) const;

protected:
    std::ostream& op_lsh(std::ostream&) const;
    soln_t _sat() const;

    virtual string const opname() const = 0;
    virtual bx_t _simplify() const = 0;
    virtual bx_t eqvar(var_t const &) const = 0;
    virtual op_t from_args(vector<bx_t> const &&) const = 0;

    op_t transform(std::function<bx_t(bx_t const &)>) const;

public:
    bool const simple;
    vector<bx_t> const args;

    Operator(Kind kind, bool simple, vector<bx_t> const & args);
    Operator(Kind kind, bool simple, vector<bx_t> const && args);

    uint32_t depth() const;
    uint32_t size() const;

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t simplify() const;
    bx_t tseytin(Context&, string const & = "a") const;
    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;

    bool is_clause() const;
};


class LatticeOperator : public Operator
{
public:
    LatticeOperator(Kind kind, bool simple, vector<bx_t> const & args);

    bx_t to_latop() const;
};


class Nor : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Nor(bool simple, vector<bx_t> const & args);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Or : public LatticeOperator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Or(bool simple, vector<bx_t> const & args);
    Or(bool simple, vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;
};


class Nand : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Nand(bool simple, vector<bx_t> const & args);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class And : public LatticeOperator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    And(bool simple, vector<bx_t> const & args);
    And(bool simple, vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_posop() const;
};


class Xnor : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Xnor(bool simple, vector<bx_t> const & args);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Xor : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(const vector<bx_t>&&) const;

public:
    Xor(bool simple, vector<bx_t> const & args);
    Xor(bool simple, vector<bx_t> const && args);

    static bx_t identity();

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Unequal : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Unequal(bool simple, vector<bx_t> const & args);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Equal : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Equal(bool simple, vector<bx_t> const & args) : Operator(EQ, simple, args) {}
    Equal(bool simple, vector<bx_t> const && args) : Operator(EQ, simple, args) {}

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class NotImplies : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    NotImplies(bool simple, bx_t p, bx_t q);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Implies : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    Implies(bool simple, bx_t p, bx_t q);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class NotIfThenElse : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    NotIfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class IfThenElse : public Operator
{
protected:
    bx_t invert() const;

    string const opname() const;
    bx_t _simplify() const;
    bx_t eqvar(var_t const &) const;
    op_t from_args(vector<bx_t> const &&) const;

public:
    IfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t to_binop() const;
    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_latop() const;
    bx_t to_posop() const;
};


class Array
{
    friend Array * operator~(Array const &);
    friend Array * operator|(Array const &, Array const &);
    friend Array * operator&(Array const &, Array const &);
    friend Array * operator^(Array const &, Array const &);

    friend Array * operator+(Array const &, Array const &);
    friend Array * operator*(Array const &, size_t);
    friend Array * operator*(size_t, Array const &);

private:
    vector<bx_t> items;

public:
    Array();
    Array(vector<bx_t> const &);
    Array(vector<bx_t> const &&);
    Array(std::initializer_list<bx_t> const items);

    bx_t const & operator[](size_t) const;
    bx_t & operator[](size_t);

    size_t size() const;
    vector<bx_t>::const_iterator begin() const;
    vector<bx_t>::const_iterator end() const;

    Array * compose(var2bx_t const &) const;
    Array * restrict_(point_t const &) const;
    bool equiv(Array const &) const;

    Array * zext(size_t num) const;
    Array * sext(size_t num) const;

    bx_t or_reduce() const;
    bx_t and_reduce() const;
    bx_t xor_reduce() const;

    std::pair<Array *, Array *> lsh(Array const &) const;
    std::pair<Array *, Array *> rsh(Array const &) const;
    std::pair<Array *, Array *> arsh(size_t) const;
};


class dfs_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
    enum class Color { WHITE, GRAY, BLACK };

    std::unordered_map<bx_t, Color> colors;
    vector<bx_t> stack;

    bx_t const * p;

    void advance_one();

public:
    dfs_iter();
    dfs_iter(bx_t const &);

    bool operator==(dfs_iter const &) const;
    bool operator!=(dfs_iter const &) const;
    bx_t const & operator*() const;
    dfs_iter const & operator++();
};


class sat_iter : public std::iterator<std::input_iterator_tag, point_t>
{
    Context ctx;
    std::unordered_map<bx_t, uint32_t> lit2idx;
    std::unordered_map<uint32_t, var_t> idx2var;

    CMSat::SATSolver solver;

    CMSat::lbool sat;
    point_t point;

    bool one_soln;

    void get_soln();

public:
    sat_iter();
    sat_iter(bx_t const &);

    bool operator==(sat_iter const &) const;
    bool operator!=(sat_iter const &) const;
    point_t const & operator*() const;
    sat_iter const & operator++();
};


class space_iter : public std::iterator<std::input_iterator_tag, vector<bool>>
{
    size_t n;
    vector<bool> counter;

public:
    space_iter();
    space_iter(size_t n);

    bool operator==(space_iter const &) const;
    bool operator!=(space_iter const &) const;
    vector<bool> const & operator*() const;
    space_iter const & operator++();
    bool parity() const;
};


class points_iter : public std::iterator<std::input_iterator_tag, point_t>
{
    space_iter it;
    vector<var_t> vars;

    point_t point;

public:
    points_iter();
    points_iter(vector<var_t> const &);

    bool operator==(points_iter const &) const;
    bool operator!=(points_iter const &) const;
    point_t const & operator*() const;
    points_iter const & operator++();
};


class domain_iter : public std::iterator<std::input_iterator_tag, point_t>
{
    std::unordered_set<var_t> s;
    points_iter it;

public:
    domain_iter();
    domain_iter(bx_t const &);

    bool operator==(domain_iter const &) const;
    bool operator!=(domain_iter const &) const;
    point_t const & operator*() const;
    domain_iter const & operator++();
};


class cf_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
    bx_t f;
    points_iter it;
    bx_t cf;

public:
    cf_iter();
    cf_iter(bx_t const &, vector<var_t> const &);

    bool operator==(cf_iter const &) const;
    bool operator!=(cf_iter const &) const;
    bx_t const & operator*() const;
    cf_iter const & operator++();
};


/// Return Boolean zero.
zero_t zero();

/// Return Boolean one.
one_t one();

/// Return Boolean "logical".
log_t logical();

ill_t illogical();

bx_t nor(vector<bx_t> const &);
bx_t nor(vector<bx_t> const &&);
bx_t nor(std::initializer_list<bx_t> const);
bx_t or_(vector<bx_t> const &);
bx_t or_(vector<bx_t> const &&);
bx_t or_(std::initializer_list<bx_t> const);
bx_t nand(vector<bx_t> const &);
bx_t nand(vector<bx_t> const &&);
bx_t nand(std::initializer_list<bx_t> const);
bx_t and_(vector<bx_t> const &);
bx_t and_(vector<bx_t> const &&);
bx_t and_(std::initializer_list<bx_t> const);
bx_t xnor(vector<bx_t> const &);
bx_t xnor(vector<bx_t> const &&);
bx_t xnor(std::initializer_list<bx_t> const);
bx_t xor_(vector<bx_t> const &);
bx_t xor_(vector<bx_t> const &&);
bx_t xor_(std::initializer_list<bx_t> const);
bx_t neq(vector<bx_t> const &);
bx_t neq(vector<bx_t> const &&);
bx_t neq(std::initializer_list<bx_t> const);
bx_t eq(vector<bx_t> const &);
bx_t eq(vector<bx_t> const &&);
bx_t eq(std::initializer_list<bx_t> const);
bx_t nimpl(bx_t const &, bx_t const &);
bx_t impl(bx_t const &, bx_t const &);
bx_t nite(bx_t const &, bx_t const &, bx_t const &);
bx_t ite(bx_t const &, bx_t const &, bx_t const &);

bx_t onehot0(vector<bx_t> const &);
bx_t onehot0(vector<bx_t> const &&);
bx_t onehot0(std::initializer_list<bx_t> const);
bx_t onehot(vector<bx_t> const &);
bx_t onehot(vector<bx_t> const &&);
bx_t onehot(std::initializer_list<bx_t> const);

bx_t nor_s(vector<bx_t> const &);
bx_t nor_s(vector<bx_t> const &&);
bx_t nor_s(std::initializer_list<bx_t> const);
bx_t or_s(vector<bx_t> const &);
bx_t or_s(vector<bx_t> const &&);
bx_t or_s(std::initializer_list<bx_t> const);
bx_t nand_s(vector<bx_t> const &);
bx_t nand_s(vector<bx_t> const &&);
bx_t nand_s(std::initializer_list<bx_t> const);
bx_t and_s(vector<bx_t> const &);
bx_t and_s(vector<bx_t> const &&);
bx_t and_s(std::initializer_list<bx_t> const);
bx_t xnor_s(vector<bx_t> const &);
bx_t xnor_s(vector<bx_t> const &&);
bx_t xnor_s(std::initializer_list<bx_t> const);
bx_t xor_s(vector<bx_t> const &);
bx_t xor_s(vector<bx_t> const &&);
bx_t xor_s(std::initializer_list<bx_t> const);
bx_t neq_s(vector<bx_t> const &);
bx_t neq_s(vector<bx_t> const &&);
bx_t neq_s(std::initializer_list<bx_t> const);
bx_t eq_s(vector<bx_t> const &);
bx_t eq_s(vector<bx_t> const &&);
bx_t eq_s(std::initializer_list<bx_t> const);
bx_t nimpl_s(bx_t const &, bx_t const &);
bx_t impl_s(bx_t const &, bx_t const &);
bx_t nite_s(bx_t const &, bx_t const &, bx_t const &);
bx_t ite_s(bx_t const &, bx_t const &, bx_t const &);

bx_t operator~(bx_t const &);
bx_t operator|(bx_t const &, bx_t const &);
bx_t operator&(bx_t const &, bx_t const &);
bx_t operator^(bx_t const &, bx_t const &);
bool operator<(lit_t const &, lit_t const &);
std::ostream& operator<<(std::ostream&, bx_t const &);

Array * operator~(Array const &);
Array * operator|(Array const &, Array const &);
Array * operator&(Array const &, Array const &);
Array * operator^(Array const &, Array const &);
Array * operator+(Array const &, Array const &);
Array * operator*(Array const &, size_t);
Array * operator*(size_t, Array const &);


} // namespace boolexpr


#endif // __cplusplus


// C Foreign Function Interface (CFFI)
extern "C"
{

typedef char const * const STRING;
typedef void * const CONTEXT;
typedef void const * const BX;
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
typedef void * const PTS_ITER;
typedef void * const DOM_ITER;
typedef void * const CF_ITER;

CONTEXT boolexpr_Context_new(void);
void boolexpr_Context_del(CONTEXT);
BX boolexpr_Context_get_var(CONTEXT, STRING);

void boolexpr_String_del(STRING);

void boolexpr_Vec_del(VEC);
void boolexpr_Vec_iter(VEC);
void boolexpr_Vec_next(VEC);
BX boolexpr_Vec_val(VEC);

void boolexpr_VarSet_del(VARSET);
void boolexpr_VarSet_iter(VARSET);
void boolexpr_VarSet_next(VARSET);
BX boolexpr_VarSet_val(VARSET);

void boolexpr_Point_del(POINT);
void boolexpr_Point_iter(POINT);
void boolexpr_Point_next(POINT);
BX boolexpr_Point_key(POINT);
BX boolexpr_Point_val(POINT);

void boolexpr_Soln_del(SOLN);
bool boolexpr_Soln_first(SOLN);
POINT boolexpr_Soln_second(SOLN);

DFS_ITER boolexpr_DfsIter_new(BX);
void boolexpr_DfsIter_del(DFS_ITER);
void boolexpr_DfsIter_next(DFS_ITER);
BX boolexpr_DfsIter_val(DFS_ITER);

SAT_ITER boolexpr_SatIter_new(BX);
void boolexpr_SatIter_del(SAT_ITER);
void boolexpr_SatIter_next(SAT_ITER);
POINT boolexpr_SatIter_val(SAT_ITER);

PTS_ITER boolexpr_PointsIter_new(size_t, VARS);
void boolexpr_PointsIter_del(PTS_ITER);
void boolexpr_PointsIter_next(PTS_ITER);
POINT boolexpr_PointsIter_val(PTS_ITER);

DOM_ITER boolexpr_DomainIter_new(BX);
void boolexpr_DomainIter_del(DOM_ITER);
void boolexpr_DomainIter_next(DOM_ITER);
POINT boolexpr_DomainIter_val(DOM_ITER);

CF_ITER boolexpr_CofactorIter_new(BX, size_t, VARS);
void boolexpr_CofactorIter_del(CF_ITER);
void boolexpr_CofactorIter_next(CF_ITER);
BX boolexpr_CofactorIter_val(CF_ITER);

BX boolexpr_zero(void);
BX boolexpr_one(void);
BX boolexpr_logical(void);
BX boolexpr_illogical(void);

BX boolexpr_not(BX);
BX boolexpr_nor(size_t, BXS);
BX boolexpr_or(size_t, BXS);
BX boolexpr_nand(size_t, BXS);
BX boolexpr_and(size_t, BXS);
BX boolexpr_xnor(size_t, BXS);
BX boolexpr_xor(size_t, BXS);
BX boolexpr_neq(size_t, BXS);
BX boolexpr_eq(size_t, BXS);
BX boolexpr_nimpl(BX, BX);
BX boolexpr_impl(BX, BX);
BX boolexpr_nite(BX, BX, BX);
BX boolexpr_ite(BX, BX, BX);
BX boolexpr_onehot0(size_t, BXS);
BX boolexpr_onehot(size_t, BXS);

BX boolexpr_nor_s(size_t, BXS);
BX boolexpr_or_s(size_t, BXS);
BX boolexpr_nand_s(size_t, BXS);
BX boolexpr_and_s(size_t, BXS);
BX boolexpr_xnor_s(size_t, BXS);
BX boolexpr_xor_s(size_t, BXS);
BX boolexpr_neq_s(size_t, BXS);
BX boolexpr_eq_s(size_t, BXS);
BX boolexpr_nimpl_s(BX, BX);
BX boolexpr_impl_s(BX, BX);
BX boolexpr_nite_s(BX, BX, BX);
BX boolexpr_ite_s(BX, BX, BX);

void boolexpr_BoolExpr_del(BX);
uint8_t boolexpr_BoolExpr_kind(BX);
STRING boolexpr_BoolExpr_to_string(BX);
uint32_t boolexpr_BoolExpr_depth(BX);
uint32_t boolexpr_BoolExpr_size(BX);
bool boolexpr_BoolExpr_is_cnf(BX);
bool boolexpr_BoolExpr_is_dnf(BX);
BX boolexpr_BoolExpr_simplify(BX);
BX boolexpr_BoolExpr_to_binop(BX);
BX boolexpr_BoolExpr_to_latop(BX);
BX boolexpr_BoolExpr_to_posop(BX);
BX boolexpr_BoolExpr_tseytin(BX, CONTEXT, STRING);
BX boolexpr_BoolExpr_compose(BX, size_t, VARS, BXS);
BX boolexpr_BoolExpr_restrict(BX, size_t, VARS, CONSTS);
BX boolexpr_BoolExpr_sat(BX);
BX boolexpr_BoolExpr_to_cnf(BX);
BX boolexpr_BoolExpr_to_dnf(BX);
BX boolexpr_BoolExpr_to_nnf(BX);
bool boolexpr_BoolExpr_equiv(BX, BX);
VARSET boolexpr_BoolExpr_support(BX);

BX boolexpr_BoolExpr_smoothing(BX, size_t, VARS);
BX boolexpr_BoolExpr_consensus(BX, size_t, VARS);
BX boolexpr_BoolExpr_derivative(BX, size_t, VARS);

CONTEXT boolexpr_Literal_ctx(BX);
uint32_t boolexpr_Literal_id(BX);

bool boolexpr_Operator_simple(BX);
VEC boolexpr_Operator_args(BX);
bool boolexpr_Operator_is_clause(BX);

ARRAY boolexpr_Array_new(size_t, BXS);
void boolexpr_Array_del(ARRAY);
size_t boolexpr_Array_size(ARRAY);
BX boolexpr_Array_getitem(ARRAY, size_t);
void boolexpr_Array_setitem(ARRAY, size_t, BX);
ARRAY boolexpr_Array_getslice(ARRAY, size_t, size_t);
ARRAY boolexpr_Array_invert(ARRAY);
ARRAY boolexpr_Array_or(ARRAY, ARRAY);
ARRAY boolexpr_Array_and(ARRAY, ARRAY);
ARRAY boolexpr_Array_xor(ARRAY, ARRAY);
ARRAY boolexpr_Array_plus(ARRAY, ARRAY);
ARRAY boolexpr_Array_mul(ARRAY, size_t);
ARRAY boolexpr_Array_compose(ARRAY, size_t, VARS, BXS);
ARRAY boolexpr_Array_restrict(ARRAY, size_t, VARS, CONSTS);
bool boolexpr_Array_equiv(ARRAY, ARRAY);
ARRAY boolexpr_Array_zext(ARRAY, size_t);
ARRAY boolexpr_Array_sext(ARRAY, size_t);
BX boolexpr_Array_or_reduce(ARRAY);
BX boolexpr_Array_and_reduce(ARRAY);
BX boolexpr_Array_xor_reduce(ARRAY);
ARRAY boolexpr_ArrayPair_fst(ARRAY_PAIR);
ARRAY boolexpr_ArrayPair_snd(ARRAY_PAIR);
void boolexpr_ArrayPair_del(ARRAY_PAIR);
ARRAY_PAIR boolexpr_Array_lsh(ARRAY, ARRAY);
ARRAY_PAIR boolexpr_Array_rsh(ARRAY, ARRAY);
ARRAY_PAIR boolexpr_Array_arsh(ARRAY, size_t);

} // extern "C"


#endif // BOOLEXPR_H
