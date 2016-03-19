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


namespace boolexpr {


using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;


// Forward declarations
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
class Context;


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

using var2bx_t = unordered_map<var_t, bx_t>;
using var2op_t = unordered_map<var_t, op_t>;
using point_t = unordered_map<var_t, const_t>;

using soln_t = std::pair<bool, boost::optional<point_t>>;


class BoolExpr : public std::enable_shared_from_this<BoolExpr> {

    friend bx_t operator~(bx_t const &);
    friend std::ostream& operator<<(std::ostream&, bx_t const &);

protected:
    virtual bx_t _invert() const = 0;
    virtual std::ostream& _op_lsh(std::ostream&) const = 0;


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
    virtual uint32_t atom_count() const = 0;
    virtual uint32_t op_count() const = 0;

    virtual bool is_cnf() const = 0;
    virtual bool is_dnf() const = 0;

    virtual bx_t pushdown_not() const = 0;
    virtual bx_t simplify() const = 0;
    virtual bx_t to_binop() const = 0;
    virtual bx_t to_latop() const = 0;
    virtual bx_t tseytin(Context&, string const & = "a") const = 0;

    virtual bx_t compose(var2bx_t const &) const = 0;
    virtual bx_t restrict_(point_t const &) const = 0;

    virtual soln_t sat() const = 0;

    bx_t to_cnf() const;
    bx_t to_dnf() const;
    bx_t to_nnf() const;

    bool equiv(bx_t const &) const;
    unordered_set<var_t> support() const;
};


class Atom : public BoolExpr {
public:
    Atom(Kind kind);

    uint32_t depth() const;
    uint32_t size() const;
    uint32_t atom_count() const;
    uint32_t op_count() const;
    bool is_cnf() const;
    bool is_dnf() const;
    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;
    bx_t tseytin(Context&, string const & = "a") const;
};


class Constant : public Atom {
public:
    Constant(Kind kind);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
};


class Known : public Constant {
public:
    bool const val;

    Known(Kind kind, bool val);
};


class Zero : public Known {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    Zero();

    bool is_dnf() const;
    soln_t sat() const;
};


class One : public Known {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    One();

    bool is_cnf() const;
    soln_t sat() const;
};


class Unknown : public Constant {
public:
    Unknown(Kind kind);
};


class Logical : public Unknown {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    Logical();

    soln_t sat() const;
};


class Illogical : public Unknown {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    Illogical();

    soln_t sat() const;
};


class Literal : public Atom {
public:
    Context *ctx;
    id_t const id;

    Literal(Kind kind, Context *ctx, id_t id);

    bool is_cnf() const;
    bool is_dnf() const;
};


class Complement : public Literal {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    Complement(Context *ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
    soln_t sat() const;
};


class Variable : public Literal {
protected:
    bx_t _invert() const;
    std::ostream& _op_lsh(std::ostream&) const;

public:
    Variable(Context *ctx, id_t id);

    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
    soln_t sat() const;
};


class Operator : public BoolExpr {
protected:
    std::ostream& _op_lsh(std::ostream&) const;

public:
    bool const simple;
    vector<bx_t> const args;

    Operator(Kind kind, bool simple, vector<bx_t> const & args);
    Operator(Kind kind, bool simple, vector<bx_t> const && args);

    uint32_t depth() const;
    uint32_t size() const;
    uint32_t atom_count() const;
    uint32_t op_count() const;

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t tseytin(Context&, string const & = "a") const;
    bx_t compose(var2bx_t const &) const;
    bx_t restrict_(point_t const &) const;
    soln_t sat() const;

    virtual string const opname() const = 0;
    virtual op_t from_args(vector<bx_t> const &&) const = 0;
    virtual bx_t eqvar(var_t const &) const = 0;

    bool is_clause() const;
    op_t transform(std::function<bx_t(bx_t const &)>) const;
};


class LatticeOperator : public Operator {
public:
    LatticeOperator(Kind kind, bool simple, vector<bx_t> const & args);

    bx_t to_latop() const;
};


class Nor : public Operator {
protected:
    bx_t _invert() const;

public:
    Nor(bool simple, vector<bx_t> const & args);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Nor"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Or : public LatticeOperator {
protected:
    bx_t _invert() const;

public:
    Or(bool simple, vector<bx_t> const & args);
    Or(bool simple, vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;

    string const opname() const { return "Or"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Nand : public Operator {
protected:
    bx_t _invert() const;

public:
    Nand(bool simple, vector<bx_t> const & args);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Nand"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class And : public LatticeOperator {
protected:
    bx_t _invert() const;

public:
    And(bool simple, vector<bx_t> const & args);
    And(bool simple, vector<bx_t> const && args);

    static bx_t identity();
    static bx_t dominator();

    bool is_cnf() const;
    bool is_dnf() const;
    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;

    string const opname() const { return "And"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Xnor : public Operator {
protected:
    bx_t _invert() const;

public:
    Xnor(bool simple, vector<bx_t> const & args);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Xnor"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Xor : public Operator {
protected:
    bx_t _invert() const;

public:
    Xor(bool simple, vector<bx_t> const & args);
    Xor(bool simple, vector<bx_t> const && args);

    static bx_t identity();

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Xor"; };
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(var_t const &) const;
};


class Unequal : public Operator {
protected:
    bx_t _invert() const;

public:
    Unequal(bool simple, vector<bx_t> const & args);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Unequal"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Equal : public Operator {
protected:
    bx_t _invert() const;

public:
    Equal(bool simple, vector<bx_t> const & args) : Operator(EQ, simple, args) {}
    Equal(bool simple, vector<bx_t> const && args) : Operator(EQ, simple, args) {}

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Equal"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class NotImplies : public Operator {
protected:
    bx_t _invert() const;

public:
    NotImplies(bool simple, bx_t p, bx_t q);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "NotImplies"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class Implies : public Operator {
protected:
    bx_t _invert() const;

public:
    Implies(bool simple, bx_t p, bx_t q);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "Implies"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class NotIfThenElse : public Operator {
protected:
    bx_t _invert() const;

public:
    NotIfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "NotIfThenElse"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class IfThenElse : public Operator {
protected:
    bx_t _invert() const;

public:
    IfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    bx_t pushdown_not() const;
    bx_t simplify() const;
    bx_t to_binop() const;
    bx_t to_latop() const;

    string const opname() const { return "IfThenElse"; };
    op_t from_args(vector<bx_t> const &&) const;
    bx_t eqvar(var_t const &) const;
};


class dfs_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
    vector<bx_t> stack;

protected:
    enum class Color { WHITE, GRAY, BLACK };
    unordered_map<bx_t, Color> colors;
    bx_t const * p;
    void _advance_one();

public:
    dfs_iter();
    dfs_iter(bx_t const &);

    bool operator==(dfs_iter const &) const;
    bool operator!=(dfs_iter const &) const;
    bx_t const & operator*() const;
    dfs_iter const & operator++();
};


class Context
{
    friend class Complement;
    friend class Variable;

    id_t id;

    unordered_map<string, var_t> vars;
    unordered_map<id_t, string> id2name;
    unordered_map<id_t, lit_t> id2lit;

    string get_name(id_t id) const;
    lit_t get_lit(id_t id) const;

public:
    Context();

    var_t get_var(string name);
};


dfs_iter begin(bx_t const &);
dfs_iter const end(bx_t const &);

zero_t zero();
one_t one();
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

//vector<bx_t> cofactors(bx_t const &, vector<var_t>&);
//bx_t smoothing(bx_t const &, vector<var_t>&);
//bx_t consensus(bx_t const &, vector<var_t>&);
//bx_t derivative(bx_t const &, vector<var_t>&);


} // namespace boolexpr


#endif // __cplusplus


// C Foreign Function Interface (CFFI)
extern "C"
{
    void * boolexpr_Context_new(void);
    void boolexpr_Context_del(void *);
    void const * boolexpr_Context_get_var(void *, char const *);

    void const * boolexpr_zero(void);
    void const * boolexpr_one(void);
    void const * boolexpr_logical(void);
    void const * boolexpr_illogical(void);

    void const * boolexpr_not(void const *);
    void const * boolexpr_nor(uint32_t, void const **);
    void const * boolexpr_or(uint32_t, void const **);
    void const * boolexpr_nand(uint32_t, void const **);
    void const * boolexpr_and(uint32_t, void const **);
    void const * boolexpr_xnor(uint32_t, void const **);
    void const * boolexpr_xor(uint32_t, void const **);
    void const * boolexpr_neq(uint32_t, void const **);
    void const * boolexpr_eq(uint32_t, void const **);
    void const * boolexpr_impl(void const *, void const *);
    void const * boolexpr_ite(void const *, void const *, void const *);
    void const * boolexpr_nor_s(uint32_t, void const **);
    void const * boolexpr_or_s(uint32_t, void const **);
    void const * boolexpr_nand_s(uint32_t, void const **);
    void const * boolexpr_and_s(uint32_t, void const **);
    void const * boolexpr_xnor_s(uint32_t, void const **);
    void const * boolexpr_xor_s(uint32_t, void const **);
    void const * boolexpr_neq_s(uint32_t, void const **);
    void const * boolexpr_eq_s(uint32_t, void const **);
    void const * boolexpr_impl_s(void const *, void const *);
    void const * boolexpr_ite_s(void const *, void const *, void const *);

    void boolexpr_BoolExpr_del(void const *);
    uint32_t boolexpr_BoolExpr_kind(void const *);
    char const * boolexpr_BoolExpr_to_string(void const *);
    void boolexpr_string_del(char const *);
    uint32_t boolexpr_BoolExpr_depth(void const *);
    uint32_t boolexpr_BoolExpr_size(void const *);
    uint32_t boolexpr_BoolExpr_atom_count(void const *);
    uint32_t boolexpr_BoolExpr_op_count(void const *);
    bool boolexpr_BoolExpr_is_cnf(void const *);
    bool boolexpr_BoolExpr_is_dnf(void const *);
    void const * boolexpr_BoolExpr_pushdown_not(void const *);
    void const * boolexpr_BoolExpr_simplify(void const *);
    void const * boolexpr_BoolExpr_to_binop(void const *);
    void const * boolexpr_BoolExpr_to_latop(void const *);
    void const * boolexpr_BoolExpr_tseytin(void const *, void *, char const *);
    void const * boolexpr_BoolExpr_to_cnf(void const *);
    void const * boolexpr_BoolExpr_to_dnf(void const *);
    void const * boolexpr_BoolExpr_to_nnf(void const *);
    bool boolexpr_BoolExpr_equiv(void const *, void const *);
}


#endif // BOOLEXPR_H
