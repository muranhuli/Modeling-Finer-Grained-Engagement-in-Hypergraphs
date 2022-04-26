#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>
using namespace std;

typedef pair<int, int> PII;

void getGraph(const string &filename, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    ifstream fin(filename, ios::in);
    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int tmp;
        vector<int> e;
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

struct myCmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

void hCoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    unordered_map<int, unordered_map<int, int>> nbr;
    set<pair<int, int>, myCmp> s;
    for (auto &it : hyperNode)
    {
        for (auto &edge : it.second)
        {
            for (auto &node : hyperEdge[edge])
            {
                nbr[it.first][node]++;
            }
        }
        nbr[it.first].erase(it.first);
        s.insert(make_pair(it.first, nbr[it.first].size()));
    }
    int k = 0;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    while (!s.empty())
    {
        pair<int, int> p = *s.begin();
        s.erase(s.begin());
        k = max(k, p.second);
        core[p.first] = k;
        visitNode[p.first] = true;
        for (auto &edge : hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;

            for (int i = 0; i < hyperEdge[edge].size(); i++)
            {
                int v = hyperEdge[edge][i];
                if (v == p.first)
                    continue;
                if (!visitNode[v])
                    s.erase(make_pair(v, nbr[v].size()));
                for (int j = 0; j < hyperEdge[edge].size(); j++)
                {
                    int u = hyperEdge[edge][j];
                    if (v == u)
                        continue;
                    nbr[v][u]--;
                    if (nbr[v][u] == 0)
                        nbr[v].erase(u);
                }
                if (!visitNode[v])
                    s.insert(make_pair(v, nbr[v].size()));
            }
        }
        nbr[p.first].clear();
    }
}

void kcoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    set<pair<int, int>, myCmp> node_count;
    unordered_map<int, int> deg;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    for (auto &it : hyperNode)
    {
        deg[it.first] = it.second.size();
        node_count.insert(make_pair(it.first, deg[it.first]));
        visitNode[it.first] = false;
    }
    int K = 0;
    while (!node_count.empty())
    {
        pair<int, int> p = *node_count.begin();
        node_count.erase(node_count.begin());
        K = max(K, p.second);
        core[p.first] = K;
        visitNode[p.first] = true;
        for (auto &edge : hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (visitNode[node])
                    continue;
                if (node_count.erase(make_pair(node, deg[node])))
                {
                    deg[node]--;
                    node_count.insert(make_pair(node, deg[node]));
                }
            }
        }
    }
}

void get_kh(string &filename, unordered_map<int, PII> &cocore)
{
    cocore.clear();
    ifstream fin(filename);
    while (true)
    {
        string str;
        getline(fin, str);
        istringstream ss(str);
        if (str == "")
            break;
        int a, b, c;
        ss >> a >> b >> c;
        cocore[a] = make_pair(b, c);
    }
}

void get_k_h(string &filename, unordered_map<int, int> &core)
{
    core.clear();
    ifstream fin(filename);
    while (true)
    {
        string str;
        getline(fin, str);
        istringstream ss(str);
        if (str == "")
            break;
        int a, b;
        ss >> a >> b;
        core[a] = b;
    }
}
/**
 * @brief 求一个超图的节点平均degree
 * 
 * @param hyperEdge 超边集
 * @param hyperNode 超点集
 * @param visitedNode 已访问的点
 * @param visitedEdge 已访问的边
 * @return float 返回超图的节点平均degree
 */
float average_hypernode_degree(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, bool> &visitedNode, vector<bool> &visitedEdge)
{
    int degree = 0;
    int count = 0;
    for (auto &it : hyperNode)
    {
        if (visitedNode[it.first])
            continue;
        ++count;
        for (auto &edge : it.second)
            if (!visitedEdge[edge])
                ++degree;
    }
    return degree * 1.0 / count;
}

float average_hypernode_edge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, bool> &visitedNode, vector<bool> &visitedEdge)
{
    int node = 0;
    int edge = 0;
    for (auto &it : hyperNode)
    {
        if (!visitedNode[it.first])
            ++node;
    }
    for (int i = 0; i < visitedEdge.size(); i++)
    {
        if (visitedEdge[i] == false)
            ++edge;
    }
    return edge * 1.0 / node;
}

float average_hyperedge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, bool> &visitedNode, vector<bool> &visitedEdge)
{
    int edge=0;
    int count=0;
    for (int i = 0; i < visitedEdge.size(); i++)
    {
        if (visitedEdge[i] == false)
        {
            count+=int(hyperEdge[i].size());
            ++edge;
        }
            
    }
    return count * 1.0 / edge;
}


const int num = 10;
int main()
{
    string file = "S_DrAb  S_MaAn  S_MiAc S_TrCl  S_WaTr  T_CoMH  T_ThAU  T_ThMS T_DBLP T_congress-bills";
    string filepath = "/home/liuyu/C++Projects/graphData/";
    istringstream ss(file);
    string str;
    while (ss >> str)
    {
        string filename = filepath + str;
        vector<vector<int>> hyperEdge;
        unordered_map<int, vector<int>> hyperNode;
        getGraph(filename, hyperEdge, hyperNode);
        cout << str << " has read!" << endl;
        cout << "edge is " << hyperEdge.size() << "  node is " << hyperNode.size() << endl;
        unordered_map<int, bool> visitedNode;
        vector<bool> visitedEdge(int(hyperEdge.size()), false);
        unordered_map<int, int> core;
        string s = "result/" + str + "/" + str + "-k-core.txt";
        get_k_h(s, core);
        for (auto &it : core)
        {
            if (it.second < num)
            {
                visitedNode[it.first] = true;
                for (auto &edge : hyperNode[it.first])
                {
                    visitedEdge[edge] = true;
                }
            }
        }
        cout << "k-h-kh-core: " << average_hyperedge(hyperEdge, hyperNode, visitedNode, visitedEdge) << ' ';
        s = "result/" + str + "/" + str + "-h-core.txt";
        get_k_h(s, core);
        visitedNode.clear();
        vector<bool>(int(hyperEdge.size()), false).swap(visitedEdge);
        for (auto &it : core)
        {
            if (it.second < num)
            {
                visitedNode[it.first] = true;
                for (auto &edge : hyperNode[it.first])
                {
                    visitedEdge[edge] = true;
                }
            }
        }
        cout << average_hyperedge(hyperEdge, hyperNode, visitedNode, visitedEdge) << ' ';
        unordered_map<int, PII> cocore;
        s = "result/" + str + "/" + str + "-khcore.txt";
        get_kh(s, cocore);
        visitedNode.clear();
        vector<bool>(int(hyperEdge.size()), false).swap(visitedEdge);
        for (auto &it : cocore)
        {
            if (it.second.first < num || (it.second.first == num && it.second.second < num))
            {
                visitedNode[it.first] = true;
                for (auto &edge : hyperNode[it.first])
                {
                    visitedEdge[edge] = true;
                }
            }
        }
        cout <<  average_hyperedge(hyperEdge, hyperNode, visitedNode, visitedEdge) << endl;
        cout<<"***********************************************"<<endl;
    }
}