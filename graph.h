//
// Created by Wang Kaixin on 2022/6/10.
//

#ifndef DESCOL_GRAPH_H
#define DESCOL_GRAPH_H

#include <set>

using namespace std;

class edge_t {
public:
    unsigned s{}, t{};

    edge_t();
    edge_t(unsigned s, unsigned t);
};


class graph_t {

public:
    unsigned long long n{}, m{};      // number of nodes/edges
    edge_t *E{};      // list of edges

    unsigned *d{};       // d: degrees of G
    unsigned *cd{};       // cumulative degree: (starts with 0) length=n+1
    unsigned *adj{};      // truncated list of neighbors
    long *new2old{};      // newID (start from 0 to n-1) - oldID (anything)

    graph_t();
    ~graph_t();

    void er_model(const char* write, long n, double p);
    void ff_model(const char* write, long n, double p);
    void clean(const char* read, const char* write);
    void read_edges(const char* read);
    void make_graph();
    void release_graph();


//    void partial_release_graph(unsigned k) const;
//    void list_k_clique_index(unsigned l, unsigned long long* n);
//    void find_clique(unsigned l, bool* found, set<unsigned>& res);
//    bool maximum_clique(set<unsigned>& m_clique);
//    void enlarge(const set<unsigned> m_clique, bool* h_table);
//    void shrink(const bool* h_table);

};

#endif //DESCOL_GRAPH_H
