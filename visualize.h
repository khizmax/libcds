#define protected public
#define private   public

#include "cds/container/hamt.h"
#include <cds/init.h>  //cds::Initialize и cds::Terminate
#include <cds/gc/hp.h> //cds::gc::HP (Hazard Pointer)
#include <iostream> //cds::gc::HP (Hazard Pointer)
#include <cds/container/optimistic_queue.h> // cds::gc::HP (Hazard Pointer)
#include <cds/container/hamt.h> // cds::gc::HP (Hazard Pointer)
#include <fstream>

#undef protected
#undef private

#include <fstream>
#include <boost/graph/graphviz.hpp>

using namespace std;

int counter = 0;
const int MAX_EDGE_COUNT = 10000;
const int MAX_HEIGHT_COUNT = 10000;

string toString(int k) {
    return to_string(k);
}

string toString(string k) {
    return k;
}

template<class K, class V>
struct Height {
    Height(typename  cds::container::Hamt<cds::gc::HP, K, V>::Node *v) {
        this->v = v;
        id = counter++;
    }

    int id;
    typename cds::container::Hamt<cds::gc::HP, K, V>::Node *v{};

    string getLabel() {
        switch (v->type) {
            case  cds::container::Hamt<cds::gc::HP, K, V>::INODE: {
                return "I" + to_string(id);
            }
            case  cds::container::Hamt<cds::gc::HP, K, V>::CNODE: {
                string label = "C" + to_string(id);
                if (static_cast<typename cds::container::Hamt<cds::gc::HP, K, V>::CNode *>(v)->isTomb) {
                    label += " - tombed";
                }
                return label;
            }
            case cds::container::Hamt<cds::gc::HP, K, V>::SNODE: {
                string label = "";
                label += "hs: " + to_string(static_cast<typename cds::container::Hamt<cds::gc::HP, K, V>::SNode *>(this->v)->getHash()) + "\n\n";
                for (auto &p: static_cast<typename cds::container::Hamt<cds::gc::HP, K, V>::SNode *>(v)->pair) {
                    label += "(" + toString(p.key) + ", " + toString(p.value) + ") \n";
                }
                return label;
            }
            default:
                return "Unknown";
        }
    }
};


struct Edge {
    string label;
    int from;
    int to;
};

template<class K, class V>
void walk_and_collect(Height<K,V> currHeight, vector<Height<K,V>> &hs, vector<Edge> &es) {
    if (currHeight.v->type == cds::container::Hamt<cds::gc::HP, K, V>::INODE) {
        Height<K,V> c(static_cast<typename cds::container::Hamt<cds::gc::HP, K, V>::INode*>(currHeight.v)->main);
        hs.push_back(c);

        es.push_back({"", currHeight.id, c.id});

        for (uint8_t path = 0b00000; path < 0b100000; path++) {
            auto *n = reinterpret_cast<typename cds::container::Hamt<cds::gc::HP, K, V>::CNode *>(c.v);
            if (n->getSubNode(path) != nullptr) {
                Height<K,V> child(n->getSubNode(path));
                hs.push_back(child);
                es.push_back({to_string(path), c.id, child.id});
                walk_and_collect(child, hs, es);
            }
        }
    }
}


template<class K, class V>
void visualize(ofstream &f, cds::container::Hamt<cds::gc::HP, K, V> *trie) {
    vector<Height<K,V>> heights = {trie->getRoot()};
    vector<Edge> edges;
    walk_and_collect(heights[0], heights, edges);

    typedef pair<int, int> Edge;
    Edge used_by[MAX_EDGE_COUNT];

    const int nedges = edges.size();
    int weights[MAX_EDGE_COUNT];
    std::fill(weights, weights + nedges, 34);

    using namespace boost;

    string names[MAX_HEIGHT_COUNT];
    for (size_t i = 0; i < heights.size(); i++) {
        names[i] = heights[i].getLabel();
    }

    typedef adjacency_list<vecS, vecS, directedS,
            property<vertex_color_t, default_color_type>,
            property<edge_weight_t, int>
    > Graph;
    Graph g(used_by, used_by, weights, heights.size());

    std::map<Graph::edge_descriptor, std::string> ename;
    for (auto &edge: edges) {
        ename[add_edge(edge.from, edge.to, g).first] = edge.label;
    }

    write_graphviz(f, g, make_label_writer(names), make_label_writer(boost::make_assoc_property_map(ename)));
}

