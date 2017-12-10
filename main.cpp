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
typedef boost::graph_traits<MyGraph>::vertex_iterator VertexItr;

vector<int> everyone;

struct connection_t{
    int id;
    int mutualFriends = 0;
};

bool compareMutualFriends(const connection_t &a, const connection_t &b)
{
    return a.mutualFriends > b.mutualFriends;
}

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
            everyone.emplace_back(vs);
            everyone.emplace_back(vt);
            boost::add_edge(vs, vt, g);
        }
        ifs.close();
    } else {
        cout << "File couldn't be opened." << endl;
    }

    sort(everyone.begin(), everyone.end());
    vector<int>::iterator it;
    it = unique(everyone.begin(), everyone.end());
    everyone.resize(distance(everyone.begin(), it));

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

    return fOfF;
}

bool binarySearch(int key, const vector<int>& b) {
    int low = 0;
    int high = b.size() - 1;
    while(high >= low)
    {
        int middle = (low + high) / 2;
        if(b.at(middle) == key)
        {
            return true;
        }
        if(b.at(middle) < key)
        {
            low = middle + 1;
        }
        if(b.at(middle) > key)
        {
            high = middle - 1;
        }
    }
    return false;
}

int countIntersection(const vector<int>&a, const vector<int>& b) {
    vector<int> result;
    for (int i = 0; i < a.size(); i++)
    {
        if (binarySearch(a.at(i), b))
        {
            result.emplace_back(a.at(i));
        }
    }

    vector<int>::iterator it;
    it = unique(result.begin(), result.end());
    result.resize(distance(result.begin(), it));

    return result.size();
}

vector<int> intersection(const vector<int>&a, const vector<int>& b) {
    vector<int> result;
    for (int i = 0; i < a.size(); i++)
    {
        if (binarySearch(a.at(i), b))
        {
            result.emplace_back(a.at(i));
        }
    }

    vector<int>::iterator it;
    it = unique(result.begin(), result.end());
    result.resize(distance(result.begin(), it));

    return result;
}

vector<connection_t> sortedFriends(const vector<int>& KsFriends, const vector<vector<int>>& friendsFriends){
    vector<connection_t> KsNetwork;
    KsNetwork.resize(KsFriends.size());
    for (int i = 0; i < KsFriends.size(); ++i) {
        KsNetwork.at(i).id = KsFriends.at(i);
        KsNetwork.at(i).mutualFriends = countIntersection(friendsFriends.at(i), KsFriends);
    }

    sort(KsNetwork.begin(), KsNetwork.end(), compareMutualFriends);

    return KsNetwork;

}

void topNFriends(MyGraph& g, int k, int N) {
    if (N < 0) {
        cout << "N must be a positive integer" << endl;
        exit(EXIT_SUCCESS);
    }
    vector<int> kFriends = findFriends(g, k);
    vector<vector<int>> kFriendsOfFriends = friendsOfFriends(g, kFriends);
    vector<connection_t> ksortedFriends = sortedFriends(kFriends, kFriendsOfFriends);

    if (N > ksortedFriends.size()) {
        if (ksortedFriends.size() == 1) {
            cout << k << " only has " << ksortedFriends.size() << " friend." << endl;
            cout << "(S)He is:" << endl;
        } else {
            cout << k << " only has " << ksortedFriends.size() << " friends." << endl;
            cout << k << "'s top " << ksortedFriends.size() << " friends are:" << endl;
        }
        N = ksortedFriends.size();
    } else {
        if (N == 1) {
            cout << k << "'s top friend is:" << endl;
        } else {
            cout << k << "'s top " << N << " friends are:" << endl;
        }
    }

    for (int i = 0; i < N; ++i) {
	    cout << ksortedFriends.at(i).id << endl;
    }
}

vector<int> notFriends(const vector<int>& friends) {
    vector<int> result;
    for (int i = 0; i < everyone.size(); i++)
    {
        if (!binarySearch(everyone.at(i), friends))
        {
            result.emplace_back(everyone.at(i));
        }
    }

    vector<int>::iterator it;
    it = unique(result.begin(), result.end());
    result.resize(distance(result.begin(), it));

    return result;
}

void TopMutualFriends(MyGraph& g, int k, int N) {
    if (N < 0) {
        cout << "N must be a positive integer" << endl;
        exit(EXIT_SUCCESS);
    }
    vector<int> kFriends = findFriends(g, k);
    vector<vector<int>> kFriendsOfFriends = friendsOfFriends(g, kFriends);
    vector<int> friendNot = notFriends(kFriends);
    friendNot.erase(find(friendNot.begin(), friendNot.end(), k));
    vector<vector<int>> resultSet;
	resultSet.resize(kFriendsOfFriends.size());

	for(int i = 0; i < kFriendsOfFriends.size(); i++) {
		resultSet.at(i) = intersection(kFriendsOfFriends.at(i), friendNot);
	}

    cout << "Result Set:" << endl;
    for (int i = 0; i < resultSet.size(); i++) {
        cout << kFriends.at(i) << ":\t";
        for (int j = 0; j < resultSet.at(i).size(); j++) {
            cout << resultSet.at(i).at(j) << " ";
        }
        cout << endl;
    }

}

int main() {
    ifstream ifs("suggest_friends.txt");
    MyGraph g;
    populateNetwork(g, ifs);
    int k = 0;
	int N = 2;

    if (!findvertex(g, k)) {
        cout << k << " is not in the network." << endl;
        exit(EXIT_SUCCESS);
    }
	//topNFriends(g, k, N);
    TopMutualFriends(g, k, N);
}
