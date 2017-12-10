#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <stdlib.h>

using namespace std;

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> MyGraph;
typedef boost::graph_traits<MyGraph>::vertex_descriptor MyVertex;
typedef boost::graph_traits <MyGraph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<MyGraph, boost::vertex_index_t>::type IndexMap;
using VertexItr = boost::graph_traits<MyGraph>::vertex_iterator; // Define Vertex iterator

struct connection_t{
    int id;
    int mutualFriends = 0;
    bool operator()(const int & c) const {
        return id == c;
    }
};

bool findvertex(const MyGraph& g, int k){
    VertexItr vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(g);
    for (; vi != vi_end; vi++) {
        if (*vi == k) {
            return true;
        }
    }
    return false;
}

void populateNetwork(MyGraph& g, ifstream& ifs) {
    string line;
    string first;
    string second;
    int vs;
    int vt;
    int pos;
    string SPACE_DELIM = " ";
    if(ifs.is_open()) {
        while(getline(ifs, line)) {
            pos = line.find(SPACE_DELIM);
            first = line.substr(0, line.find(SPACE_DELIM));
            line.erase(0, pos + SPACE_DELIM.length());
            second = line;
            vs = atoi(first.c_str());
            vt = atoi(second.c_str());
            boost::add_edge(vs, vt, g);
        }
        ifs.close();
    } else {
        cout << "File couldn't be opened." << endl;
    }
}

vector<int> findFriends(const MyGraph& g, const int& k) {
    vector<int> friends;

    IndexMap index = get(boost::vertex_index, g);
    pair<adjacency_iterator, adjacency_iterator> neighbors =
    boost::adjacent_vertices(boost::vertex(k, g), g);
    for(; neighbors.first != neighbors.second; ++neighbors.first) {
        friends.emplace_back(index[*neighbors.first]);
    }

    sort(friends.begin(), friends.end());
    return friends;
}


vector<vector<int>> friendsOfFriends(const MyGraph& g, vector<int>& v) {
	vector<vector<int>> fOfF;

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        fOfF.push_back(findFriends(g, *it));
    }

    return fOF;
}

bool compareMutualFriends(const connection_t &a, const connection_t &b)
{
    return a.mutualFriends > b.mutualFriends;
}

int main() {
    ifstream ifs("sample_edges.txt");
    MyGraph g;
    populateNetwork(g, ifs);
    int k = 0;

    if (!findvertex(g, k)) {
        cout << k << " is not in the network." << endl;
        exit(EXIT_SUCCESS);
    }

    vector<int> output = findFriends(g, k);
    vector<vector<int>> t = friendsOfFriends(g, output);

}
