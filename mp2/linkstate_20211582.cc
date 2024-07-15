#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <utility>
#include <sstream>
#include <queue>
#define MAX_NODE 100
#define MAX_COST 100
#define MAX_WORD 100
#define INF 1e9
using namespace std;

struct compare{
    bool operator()(pair<int, int> A, pair<int, int> B){
        if (A.first == B.first) return A.second > B.second;
        return A.first > B.first;
    }
};

int main(int argc, char *argv[]){
    if(argc != 4){
        cout << "usage: linkstate topologyfile messagesfile changesfile\n";
        return 0;
    }
    
    //file open
    ifstream topology(argv[1]);
    if(!topology.is_open()) {
        cout << "Error: open input file.\n";
        return 0;
    }
    ifstream message(argv[2]);
    if(!message.is_open()) {
        cout << "Error: open input file.\n";
        return 0;
    }
    ifstream change(argv[3]);
    if(!change.is_open()) {
        cout << "Error: open input file.\n";
        return 0;
    }
        
    //distance vector init
    int nodenum = 0;
    topology >> nodenum;
    int distance[MAX_NODE][MAX_NODE];
    for (int i = 0 ; i < MAX_NODE; i++){
        for (int j = 0; j < MAX_NODE; j++){
            if (i == j) distance[i][j] = 0;
            distance[i][j]= INF;
        }
    }
    int next[MAX_NODE][MAX_NODE];
    for (int i = 0 ; i < MAX_NODE; i++){
        for (int j = 0; j < MAX_NODE; j++){
			next[i][j]= INF;
        }
    }

	vector <vector<pair<int, int> > > graph(MAX_NODE);
    int node1, node2, w;
    while(topology >> node1 >> node2 >> w){
        graph[node1].push_back(make_pair(w, node2));
		graph[node2].push_back(make_pair(w, node1));
    }
    topology.close();

    //dijkstra
    for (int i = 0; i < nodenum; i++) {
        distance[i][i] = 0;
        next[i][i] = i;
    }

    for (int i = 0; i < nodenum; i++) {
		priority_queue<pair<int, int>, vector<pair<int, int> >, compare> PQ;
		PQ.push(make_pair(0,i));
		while (!PQ.empty()){
			int weight = PQ.top().first;
			int cur = PQ.top().second;
			PQ.pop();
            for (size_t j = 0; j < graph[cur].size(); j++){
                int new_weight = graph[cur][j].first;
                int new_node = graph[cur][j].second;
                if(distance[i][new_node] > weight + new_weight) {
                    next[i][new_node] = cur;
                    distance[i][new_node] = weight + new_weight;
                    PQ.push(make_pair(distance[i][new_node], new_node));
                }
                else if ((distance[i][new_node] == weight + new_weight) &&(next[i][new_node] > cur)) {
                    next[i][new_node] = cur;
                }
            }
		}
	}
    
    //write routing table
    ofstream fout("output_ls.txt");
    if(!fout.is_open()) {
        cout << "Error: open output file.\n";
        return 0;
    }

    for(int i = 0; i < nodenum; i++){
        for(int j = 0; j < nodenum; j++){
            if(distance[i][j] == INF) continue;
            if(distance[i][j] == 0){
                fout << j << ' ' << next[i][j] << ' ' << distance[i][j] << '\n';
                continue;    
            }
            int cur = j;
            while(1){
                if (next[i][cur] == i) break;
                cur = next[i][cur];
                
            }
            fout << j << ' ' << cur << ' ' << distance[i][j] << '\n';
        }
        fout << '\n';
    }

    //messages.txt
    string line;
    while(getline(message, line)){
        istringstream iss(line);
        int n1, n2;
        string message;
        iss >> n1 >> n2;
        getline(iss, message); 
        fout << "from " << n1 << " to " << n2 << " cost " << distance[n1][n2] << " hops ";

        string route;
        int cur = n1;
        while(1){
            fout << cur << ' ';
            if (next[n2][cur] == n2) break;
            cur = next[n2][cur];
        }
        fout << "message" << message << '\n';
    }
    message.close();
    fout << '\n';
    
    //changes.txt
    while(change >> node1 >> node2 >> w){
        vector<pair<int, int> >::iterator iter;
        for (iter = graph[node2].begin(); iter != graph[node2].end(); iter++){
            if ((*iter).second == node1) {
                graph[node2].erase(iter);
                break;
            }
        }
        vector<pair<int, int> >::iterator iter2;
        for (iter2 = graph[node1].begin(); iter2 != graph[node1].end(); iter2++){
            if ((*iter2).second == node2) {
                graph[node1].erase(iter2);
                break;
            }
        } 
        if (1<= w && w <= 100) {//새로운 edge가 추가됨.
            graph[node1].push_back(make_pair(w, node2));
		    graph[node2].push_back(make_pair(w, node1));
        }

        for (int i = 0 ; i < MAX_NODE; i++){
            for (int j = 0; j < MAX_NODE; j++){
                if (i == j) distance[i][j] = 0;
                distance[i][j]= INF;
            }
        }
        for (int i = 0 ; i < MAX_NODE; i++){
            for (int j = 0; j < MAX_NODE; j++){
			    next[i][j]= INF;
            }
        }
        
        for (int i = 0; i < nodenum; i++) {
            distance[i][i] = 0;
            next[i][i] = i;
        }

        for (int i = 0; i < nodenum; i++) {
            priority_queue<pair<int, int>, vector<pair<int, int> >, compare> PQ;
		    PQ.push(make_pair(0,i));
		    while (!PQ.empty()){
			    int weight = PQ.top().first;
			    int cur = PQ.top().second;
			    PQ.pop();
                for (size_t j = 0; j < graph[cur].size(); j++){
                    int new_weight = graph[cur][j].first;
                    int new_node = graph[cur][j].second;
                    if(distance[i][new_node] > weight + new_weight) {
                        next[i][new_node] = cur;
                        distance[i][new_node] = weight + new_weight;
                        PQ.push(make_pair(distance[i][new_node], new_node));
                    }
                    else if ((distance[i][new_node] == weight + new_weight) &&(next[i][new_node] > cur)) {
                        next[i][new_node] = cur;
                    }   
                }
		    }
	    }

        for(int i = 0; i < nodenum; i++){
            for(int j = 0; j < nodenum; j++){
                if(distance[i][j] == INF) continue;
                if(distance[i][j] == 0){
                    fout << j << ' ' << next[i][j] << ' ' << distance[i][j] << '\n';
                    continue;    
                }
                int cur = j;
                while(1){
                    if (next[i][cur] == i) break;
                    cur = next[i][cur];
                
                }
                fout << j << ' ' << cur << ' ' << distance[i][j] << '\n';
            }
            fout << '\n';
        }

        ifstream change_message(argv[2]);
        string change_line;
        while(getline(change_message, change_line)){
            istringstream iss(change_line);
            int n1, n2;
            string change_message;
            iss >> n1 >> n2;
            getline(iss, change_message);
            if (distance[n1][n2] == INF) {
                fout << "from " << n1 << " to " << n2;
                fout << " cost infinite hops unreachable message" << change_message << '\n';
                continue;
            }
            fout << "from " << n1 << " to " << n2 << " cost " << distance[n1][n2] << " hops ";

            string route;
            int cur = n1;
            while(1){
                fout << cur << ' ';
                if (next[n2][cur] == n2) break;
                cur = next[n2][cur];
            }
            fout << "message" << change_message << '\n';
        }
        fout << '\n';
        change_message.close();
        
    }
    change.close();
    cout << "Complete. Output file written to output_ls.txt.\n";
    fout.close();
    return 0;
}
