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


struct BoolExprProxy
{
    bx_t const bx;

    BoolExprProxy(bx_t const & bx): bx {bx} {}
};


template <typename T>
struct VecProxy
{
    vector<T> v;
    typename vector<T>::iterator it;

    VecProxy(vector<T> const & v): v {v} {}

    void iter() { it = v.begin(); }
    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == v.end()) ? nullptr
                               : new BoolExprProxy(*it);
    }
};


template <typename T>
struct SetProxy
{
    std::unordered_set<T> s;
    typename std::unordered_set<T>::iterator it;

    SetProxy(std::unordered_set<T> const && s): s {s} {}

    void iter() { it = s.begin(); }
    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == s.end()) ? nullptr
                               : new BoolExprProxy(*it);
    }
};


template <typename K, typename V>
struct MapProxy
{
    std::unordered_map<K, V> m;
    typename std::unordered_map<K, V>::iterator it;

    MapProxy(std::unordered_map<K, V> const & m): m {m} {}
    MapProxy(std::unordered_map<K, V> const && m): m {m} {}

    void iter() { it = m.begin(); }
    void next() { ++it; }

    BoolExprProxy * key() const
    {
        return (it == m.end()) ? nullptr
                               : new BoolExprProxy((*it).first);
    }

    BoolExprProxy * val() const
    {
        return (it == m.end()) ? nullptr
                               : new BoolExprProxy((*it).second);
    }
};


struct SolnProxy
{
    soln_t soln;

    SolnProxy(soln_t const && soln)
        : soln {soln}
    {}
};


struct DfsIterProxy
{
    dfs_iter it;

    DfsIterProxy(bx_t const & bx)
        : it {dfs_iter(bx)}
    {}

    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == dfs_iter()) ? nullptr
                                  : new BoolExprProxy(*it);
    }
};


struct SatIterProxy
{
    sat_iter it;

    SatIterProxy(bx_t const & bx)
        : it {sat_iter(bx)}
    {}

    void next() { ++it; }

    MapProxy<var_t, const_t> * val() const
    {
        return (it == sat_iter()) ? nullptr
                                  : new MapProxy<var_t, const_t>(*it);
    }
};


struct PointsIterProxy
{
    points_iter it;

    PointsIterProxy(vector<var_t> const & xs)
        : it {xs}
    {}

    void next() { ++it; }

    MapProxy<var_t, const_t> * val() const
    {
        return (it == points_iter()) ? nullptr
                                     : new MapProxy<var_t, const_t>(*it);
    }
};


struct DomainIterProxy
{
    domain_iter it;

    DomainIterProxy(bx_t const & bx)
        : it {domain_iter(bx)}
    {}

    void next() { ++it; }

    MapProxy<var_t, const_t> * val() const
    {
        return (it == domain_iter()) ? nullptr
                                     : new MapProxy<var_t, const_t>(*it);
    }
};


struct CofactorIterProxy
{
    cf_iter it;

    CofactorIterProxy(bx_t const & bx, vector<var_t> const & vars)
        : it {cf_iter(bx, vars)}
    {}

    void next() { ++it; }

    BoolExprProxy * val() const
    {
        return (it == cf_iter()) ? nullptr
                                 : new BoolExprProxy(*it);
    }
};


} // namespace boolexpr
