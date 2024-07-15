#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <utility>
#include <sstream>
#define MAX_NODE 100
#define MAX_COST 100
#define MAX_WORD 100
#define INF 1e9
using namespace std;

int main(int argc, char *argv[]){
    if(argc != 4){
        cout << "usage: distvec topologyfile messagesfile changesfile\n";
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
    int weight[MAX_NODE][MAX_NODE];
    for (int i = 0 ; i < MAX_NODE; i++){
        for (int j = 0; j < MAX_NODE; j++){
            if (i==j) weight[i][j] =0;
            else weight[i][j]= INF;
        }
    }
    int distance[MAX_NODE][MAX_NODE];
    for (int i = 0 ; i < MAX_NODE; i++){
        for (int j = 0; j < MAX_NODE; j++){
            if (i==j) distance[i][j] =0;
            else distance[i][j]= INF;
        }
    }
    int next[MAX_NODE][MAX_NODE];
    for (int i = 0 ; i < MAX_NODE; i++){
        for (int j = 0; j < MAX_NODE; j++){
            if (i==j) next[i][j] = i;
            else next[i][j]= INF;
        }
    }
    int node1, node2, w;
    while(topology >> node1 >> node2 >> w){
        weight[node1][node2] = w;
        weight[node2][node1] = w;
    }
    topology.close();

    //distance vector update
    int flag;
    while(1){
        flag = 0;
        for (int i = 0 ; i < nodenum; i++){
            for (int j = 0 ; j < nodenum; j++){
                for (int k = 0 ; k < nodenum; k++){
                    if (distance[i][k] > distance[i][j] + weight[j][k]) {
                        next[i][k] = j;
                        distance[i][k] = distance[i][j] + weight[j][k];
                        flag = 1;
                    }
                    else if (j != k && distance[i][k] == distance[i][j] + weight[j][k]){
                        if (next[i][k] > j) {
                            next[i][k] = j;
                            flag = 1;
                        }
                    }
                }
            }
        }
        if (!flag) break;
    }
    
    //write routing table
    ofstream fout("output_dv.txt");
    if(!fout.is_open()) {
        cout << "Error: open output file.\n";
        return 0;
    }

    for(int i = 0; i < nodenum; i++){
        for(int j = 0; j < nodenum; j++){
            if(distance[i][j] == INF) continue;
            if(distance[i][j] == 0){
                fout << i << ' ' << next[i][j] << ' ' << distance[i][j] << '\n';
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
        if(w == -999){
            weight[node1][node2] = INF;
            weight[node2][node1] = INF;   
        }
        else{
            weight[node1][node2] = w;
            weight[node2][node1] = w;
        }
        for (int i = 0 ; i < MAX_NODE; i++){
            for (int j = 0; j < MAX_NODE; j++){
                if (i==j) distance[i][j] =0;
                else distance[i][j]= INF;
            }
        }
        for (int i = 0 ; i < MAX_NODE; i++){
            for (int j = 0; j < MAX_NODE; j++){
                if (i==j) next[i][j] = i;
                else next[i][j]= INF;
            }
        }
        
        int change_flag;
        while(1){
            change_flag = 0;
            for (int i = 0 ; i < nodenum; i++){
                for (int j = 0 ; j < nodenum; j++){
                    for (int k = 0 ; k < nodenum; k++){
                        if (distance[i][k] > distance[i][j] + weight[j][k]) {
                            next[i][k] = j;
                            distance[i][k] = distance[i][j] + weight[j][k];
                            change_flag = 1;
                        }
                        else if (j != k && distance[i][k] == distance[i][j] + weight[j][k]){
                            if (next[i][k] > j) {
                                next[i][k] = j;
                                change_flag = 1;
                            }
                        }
                    }
                }
            }
            if (!change_flag) break;
        }

        for(int i = 0; i < nodenum; i++){
            for(int j = 0; j < nodenum; j++){
                if(distance[i][j] == INF) continue;
                if(distance[i][j] == 0){
                    fout << i << ' ' << next[i][j] << ' ' << distance[i][j] << '\n';
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
    cout << "Complete. Output file written to output_dv.txt.\n";
    fout.close();
    return 0;
}
