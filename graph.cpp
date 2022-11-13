#include "graph.h"
#include "utils.h"
#include <cassert>
#include <map>
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <queue>
#include "defs.h"

graph_t::graph_t() = default;
graph_t::~graph_t() = default;

edge_t::edge_t() = default;
edge_t::edge_t(unsigned int s, unsigned int t) {
    this->s = s < t ? s : t;
    this->t = s < t ? t : s;
}

bool operator< (const edge_t e1, const edge_t e2) {
    return (e1.s < e2.s) || (e1.s == e2.s && e1.t < e2.t);
}

void graph_t::er_model(const char* write, long n, double p) {
    unsigned s, t;
    long long k;
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<> dist(0, 1);

    FILE *fp = fopen(write, "w");
    this->n = n;
    this->m = 0;

    k = 1 + log(dist(gen)) / log(1 - p);

    while (log(k) <= 2 * log(n)) {
        s = (k - 1) / n;
        t = (k - 1) % n;

        if (s < t) {
            this->m++;
            fprintf(fp, "%u %u\n", s, t);
        }

        k += 1 + log(dist(gen)) / log(1 - p);
    }

    fclose(fp);
}

void graph_t::ff_model(const char* write, long n, double p) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<> random(0, 1);
    map<long, vector<long>> V;

    queue<long> vertex_q;
    map<long, bool> visited;

//    edge_t *E = (edge_t*) malloc(N_EDGES * sizeof(edge_t));
    this->n = n;
    this->m = 0;
    long u, v, ambassador, index;
    double q;

//    string filename = "FF" + to_string((int)(n / 1e6)) + "M.edges";
    FILE *fp = fopen(write, "w");

    for (unsigned i = 1; i < n; i++) {
        fprintf(stdout, "%.2lf%%\r", (double) i / n * 100);
        fflush(stdout);
        uniform_int_distribution<> random_vertex(0, i-1);
        ambassador = random_vertex(gen);
        vertex_q.push(ambassador);
        visited[i] = true;
        visited[ambassador] = true;

        while (!vertex_q.empty()) {
            u = vertex_q.front();
            vector<long> neighbors (V[u]);

            while (true) {
                q = random(gen);
                if (q > p || neighbors.empty()) break;

                uniform_int_distribution<> random_neighbor(0, neighbors.size() - 1);
                index = random_neighbor(gen);
                v = neighbors[index];
                neighbors[index] = neighbors[neighbors.size() - 1];
                neighbors.pop_back();
                if (!visited[v]) {
                    vertex_q.push(v);
                    visited[v] = true;
                }
            }

            V[i].push_back(u);
            V[u].push_back(i);

            fprintf(fp, "%lu %u\n", u, i);

            this->m++;
            vertex_q.pop();
        }
        visited.clear();
    }

    fclose(fp);

}

void graph_t::clean(const char *read, const char *write) {
    set<edge_t> E;
    unsigned s, t;
    FILE *f_read = fopen(read, "r");
    FILE *f_write = fopen(write, "w");

    while (fscanf(f_read, "%u %u%*[^\n]%*c", &s, &t) == 2) {
        if (s == t) {
            printf("%u Self loop.\n", s);
            continue;
        }
        if (E.find(edge_t(s, t)) != E.end()) {
            printf("(%u, %u) duplicates.\n", s, t);
            continue;
        }

        E.insert(edge_t(s, t));
        fprintf(f_write, "%u %u\n", s, t);
    }

    fclose(f_read);
    fclose(f_write);
}


void graph_t::read_edges(const char *read) {
    FILE *fp;
    if ((fp = fopen(read, "r")) == nullptr) {
        printf("Cannot open file %s.\n", read);
        exit(0);
    }

    unsigned u, v, i, k;
    long *old2new = new long [N_NODES];
    for (i = 0; i < N_NODES; i++) old2new[i] = -1;
    this->n = this->m = 0;
    this->E = new edge_t [N_EDGES];
    this->new2old = new long [N_NODES];

    while (fscanf(fp, "%u %u", &u, &v) == 2) {

        if (u > N_NODES || v > N_NODES) {
            printf("Enlarge N_NODES to at least %u.\n", max2(u, v));
            exit(0);
        }
        if (old2new[u] == -1) {
            old2new[u] = this->n;
            this->new2old[this->n++] = u;
        }
        if (old2new[v] == -1) {
            old2new[v] = this->n;
            this->new2old[this->n++] = v;
        }

        this->E[this->m++] = edge_t(old2new[u], old2new[v]);

        if (this->m > N_EDGES) {
            printf("Enlarge N_EDGES to at least %llu.\n", this->m);
            exit(0);
        }
    }

    fclose(fp);

    delete [] old2new;
}



void graph_t::make_graph(unsigned int k) {
    unsigned i, max_deg = 0;
    auto *_od = new unsigned [this->n]();
    auto *_d = new unsigned [this->n]();
    auto *_sub = new unsigned [this->n];

    this->cd = new unsigned [this->n + 1];
    this->adj = new unsigned [this->m];
    this->lab = new unsigned [this->n];

    for (i = 0; i < this->m; i++) {
        _od[this->E[i].s]++;
        _d[this->E[i].s]++;
        _d[this->E[i].t]++;
    }
    this->cd[0] = 0;
    for (i = 1; i < this->n + 1; i++) {
        this->cd[i] = this->cd[i - 1] + _od[i - 1];
        max_deg = (_od[i - 1] > max_deg) ? _od[i - 1] : max_deg;
        _od[i - 1] = 0;
        _sub[i - 1] = i - 1;
        this->lab[i - 1] = k;
    }
    for (i = 0; i < this->m; i++) {
        unsigned s = this->E[i].s, t = this->E[i].t;
        this->adj[this->cd[s] + _od[s]++] = t;
    }

    this->ns = new unsigned [k + 1];
    this->ns[k] = this->n;

    this->d = new unsigned* [k + 1];
    this->od = new unsigned* [k + 1];
    this->sub = new unsigned* [k + 1];
    for (i = 0; i < k; i++) {
        this->d[i] = new unsigned [this->n];
        this->od[i] = new unsigned [this->n];
        this->sub[i] = new unsigned [max_deg];
    }
    this->d[k] = _d;
    this->od[k] = _od;
    this->sub[k] = _sub;

    this->core_num = max_deg;
}

void graph_t::release_graph(unsigned int k) const {
    delete [] this->E;
    delete [] this->ns;
    delete [] this->cd;
    delete [] this->adj;
    delete [] this->rank;
    delete [] this->new2old;
    delete [] this->lab;
    delete [] this->color;

    for (int i = 0; i < k + 1; ++i) {
        delete [] this->d[i];
        delete [] this->od[i];
        delete [] sub[i];
    }

    delete [] this->d;
    delete [] this->od;
    delete [] this->sub;
}
