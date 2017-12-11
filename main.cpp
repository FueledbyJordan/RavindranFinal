#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> MyGraph;
typedef boost::graph_traits<MyGraph>::vertex_descriptor MyVertex;
typedef boost::graph_traits <MyGraph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<MyGraph, boost::vertex_index_t>::type IndexMap;
typedef boost::graph_traits<MyGraph>::vertex_iterator VertexItr;

/**
	vector of all members of the graph
*/
vector<int> everyone;

/**
	@param id the identifier for vertex
	@param mutualFriends the number of friends shared
*/
struct connection_t{
    int id;
    int mutualFriends = 0;
};

/**
	@param id the identifier for vertex
	@param numOccurences the number of times friend is mutual
*/
struct friend_t {
    int id;
    int numOccurences = 0;
};

/**
	Comparator for friend_t.
 	@param a vector to compare
	@param b vector to compare
	@return true if a.numOccurences is greater than b.numOccurences
*/
bool compareNumOccurences(const friend_t &a, const friend_t &b) {
    return a.numOccurences > b.numOccurences;
}

/**
	Comparator for connection_t.
	@param a vector to compare
	@param b vector to compare
	@return true if a.mutualFriends is greater than b.mutualFriends
*/
bool compareMutualFriends(const connection_t &a, const connection_t &b) {
    return a.mutualFriends > b.mutualFriends;
}

/**
	Finds a vertex in the graph.
	@param g graph to search
	@param k vertex to find
	@return true if k exists in g
*/
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

/**
	Populate the graph with vertices and edges.
 	@param g graph to populate
	@param ifs file to extract data
*/
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

/**
	Find all vertices that share an edge with a given vertex.
	@param g graph to search
	@param k root vertex
	@return vector of verticies that share an edge with k
*/
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

/**
	Find all verticies that share an edge with vertices in a vector.
	@param g graph to search
	@param v vertex of vertices
	@return vector of vectors that contain verticies with shared edges
*/
vector<vector<int>> friendsOfFriends(const MyGraph& g, vector<int>& v) {
	vector<vector<int>> fOfF;

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        fOfF.push_back(findFriends(g, *it));
    }

    return fOfF;
}

/**
	Search sorted vector for a value. O(logn).
	@param key value to search for
	@param b vector to search
	@retrun true if key exists in b
*/
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

/**
	Search sorted vector of type friend_t for a value.
	@param key value to search for
	@param b vector to search
	@retrun true if key exists in b
*/
bool linearSearch(int key, const vector<friend_t>& b) {
    for (int i = 0; i < b.size(); i++) {
        if (key == b.at(i).id) {
            return true;
        }
    }
    return false;
}

/**
	Count the number of shared values between two vectors.
	@param a vector to compare
	@param b vector to compare
	@return size of vector containing shared values
*/
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

/**
	Find shared values between two vectors.
	@param a vector to compare
	@param b vector to compare
	@return vector containing shared values
*/
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

/**
	Sort vector of verticies by number of edges in common.
	@param KsFriends vector of verticies to sort
	@param friendsFriends vector of vectors of vertices to check for common edges
	@return sorted vector of type connection_t
*/
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

/**
	Find the top N friends of a vertex.
	@param g graph to search
	@param k root vertex
	@param N number of top friends to identify
*/
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

/**
	Find all verticies that are not friends of a vertex.
	@param vector of friends
	@return vector of verticies that do not share an edge with root vertex
*/
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
/**
	Suggest M new friends of a given vertex.
	@param g graph to search
	@param k root vertex
	@param N number of friends to suggest
*/
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
    vector<int> listOfSuggestedFriends;

	for(int i = 0; i < kFriendsOfFriends.size(); i++) {
		resultSet.at(i) = intersection(kFriendsOfFriends.at(i), friendNot);
	}

    for (int i = 0; i < resultSet.size(); i++) {
        listOfSuggestedFriends.insert(listOfSuggestedFriends.end(), resultSet.at(i).begin(), resultSet.at(i).end());
    }

	sort(listOfSuggestedFriends.begin(), listOfSuggestedFriends.end());

	vector<friend_t> suggestedConnections;
    suggestedConnections.resize(listOfSuggestedFriends.size());

    int i = 0;
    int j = 0;
    int l = 0;

    while (i < listOfSuggestedFriends.size()) {
		if (!linearSearch(listOfSuggestedFriends.at(i), suggestedConnections)) {
			suggestedConnections.at(j).id = listOfSuggestedFriends.at(i);
			suggestedConnections.at(j).numOccurences++;
            l = j;
            j++;
		} else {
			suggestedConnections.at(l).numOccurences++;
		}
        i++;
    }

    int numZeros = suggestedConnections.size() - j - 1;

    suggestedConnections.resize(j);

    friend_t fixedZero;
    fixedZero.id = 0;
    fixedZero.numOccurences = numZeros;

    suggestedConnections.emplace_back(fixedZero);

    sort(suggestedConnections.begin(), suggestedConnections.end(), compareNumOccurences);

    if (N > suggestedConnections.size()) {
        if (suggestedConnections.size() == 1) {
            cout << k << " only has " << suggestedConnections.size() << " second tier connection." << endl;
            cout << "(S)He is:" << endl;
        } else {
            cout << k << " only has " << suggestedConnections.size() << " second tier connections." << endl;
            cout << k << "'s top " << suggestedConnections.size() << " second tier connections are:" << endl;
        }
        N = suggestedConnections.size();
    } else {
        if (N == 1) {
            cout << k << "'s recommended friend is:" << endl;
        } else {
            cout << k << "'s top " << N << " recommended friends are:" << endl;
        }
    }

    for (int i = 0; i < N; ++i) {
	    cout << suggestedConnections.at(i).id << endl;
    }
}

int main() {
    ifstream ifs("facebook_combined.txt");
    MyGraph g;
    populateNetwork(g, ifs);

    int k = 946;
	int N = 20;

    if (!findvertex(g, k)) {
        cout << k << " is not in the network." << endl;
        exit(EXIT_SUCCESS);
    }

    struct timeval start, end;

    gettimeofday(&start, 0);

    topNFriends(g, k, N);
    //TopMutualFriends(g, k, N);

    gettimeofday(&end, 0);
    cout << endl;
    cout << (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)<< " us" << endl;

}
