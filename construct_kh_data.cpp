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

void hCoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, unordered_map<int, bool> &visitedNode, vector<bool> &visitedEdge)
{
    core.clear();
    unordered_map<int, unordered_map<int, int>> nbr;
    set<pair<int, int>, myCmp> s;
    for (auto &it : hyperNode)
    {
        if (visitedNode[it.first])
            continue;
        for (auto &edge : it.second)
        {
            if (visitedEdge[edge])
                continue;
            for (auto &node : hyperEdge[edge])
            {
                nbr[it.first][node]++;
            }
        }
        nbr[it.first].erase(it.first);
        s.insert(make_pair(it.first, nbr[it.first].size()));
    }
    int k = 0;
    while (!s.empty())
    {
        pair<int, int> p = *s.begin();
        s.erase(s.begin());
        k = max(k, p.second);
        core[p.first] = k;
        visitedNode[p.first] = true;
        for (auto &edge : hyperNode[p.first])
        {
            if (visitedEdge[edge])
                continue;
            visitedEdge[edge] = true;
            for (int i = 0; i < hyperEdge[edge].size(); i++)
            {
                int v = hyperEdge[edge][i];
                if (v == p.first)
                    continue;
                if (!visitedNode[v])
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
                if (!visitedNode[v])
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
                node_count.erase(make_pair(node, deg[node]));
                deg[node]--;
                node_count.insert(make_pair(node, deg[node]));
            }
        }
    }
}

/**
 * @brief 从文件中读取khcore，减少重新计算所需要的时间
 * 
 * @param filename 
 * @param cocore 
 */
void readfile_khcore(const string &filename, unordered_map<int, PII> &cocore)
{
    string s = "./result/" + filename + "/" + filename + "-khcore.txt";
    ifstream fin(s);
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int a, b, c;
        ss >> a >> b >> c;
        cocore[a] = make_pair(b, c);
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
int main()
{
    //"S_MiAc S_DrAb S_TrCl  S_WaTr  T_CoMH  T_ThAU  T_ThMS T_congress-bills T_DBLP S_MaAn"
    string file = "S_TrCl T_CoMH  T_congress-bills   T_DBLP S_DrAb S_WaTr T_ThAU  T_ThMS   "; //MaAn
    string filepath = "/home/liuyu/C++Projects/graphData/";
    istringstream ss(file);
    string str;
    int a[] = {20, 500, 150, 150, 60, 12, 6, 25};
    int i = 0;
    while (ss >> str)
    {
        string filename = filepath + str;
        vector<vector<int>> hyperEdge;
        unordered_map<int, vector<int>> hyperNode;
        getGraph(filename, hyperEdge, hyperNode);
        cout << str << " has read!" << endl;
        cout << "edge is " << hyperEdge.size() << "  node is " << hyperNode.size() << endl;
        unordered_map<int, int> core;
        string s = "result/" + str + "/" + str + "-h-core.txt";
        get_k_h(s, core);
        cout << "h-core 完成" << endl;
        unordered_map<int, bool> visitedNode;
        vector<bool> visitedEdge(int(hyperEdge.size()), false);
        int maxk = INT_MIN;
        int t = a[i++];
        cout << "t = " << t << endl;
        for (auto &it : core)
        {
            if (it.second < t)
            {
                visitedNode[it.first] = true;
                for (auto &edge : hyperNode[it.first])
                {
                    visitedEdge[edge] = true;
                }
            }
        }
        unordered_map<int, unordered_set<int>> knode2node; //表示k大于maxk/2的超图，所转化为的普通子图
        for (auto &it : hyperNode)
        {
            if (!visitedNode[it.first])
            {
                for (auto &edge : it.second)
                {
                    if (!visitedEdge[edge])
                    {
                        for (auto &node : hyperEdge[edge])
                        {
                            knode2node[it.first].insert(node);
                        }
                    }
                }
            }
        }
        ofstream fout("result/" + str + "/" + str + "-hnode2node.txt");
        for (auto &it : knode2node)
        {
            for (auto &node : it.second)
                fout << it.first << ' ' << node << endl;
        }
        fout.close();

        // int maxh = INT_MIN;
        // for (auto &it : cocore)
        // {
        //     if (it.second.first == t)
        //         maxh = max(maxh, it.second.second);
        // }
        // cout << "maxk=" << maxk << " maxh=" << maxh << endl;
        // for (auto &it : cocore)
        // {
        //     if (it.second.first == t && it.second.second < maxh)
        //     {
        //         visitedNode[it.first] = true;
        //         for (auto &edge : hyperNode[it.first])
        //         {
        //             visitedEdge[edge] = true;
        //         }
        //     }
        // }
        // cout << average_hypernode_degree(hyperEdge, hyperNode, visitedNode, visitedEdge) << endl;
        // unordered_map<int, unordered_set<int>> khnode2node; //表示k大于maxk/2的超图，所转化为的普通子图
        // for (auto &it : hyperNode)
        // {
        //     if (!visitedNode[it.first])
        //     {
        //         for (auto &edge : it.second)
        //         {
        //             if (!visitedEdge[edge])
        //             {
        //                 for (auto &node : hyperEdge[edge])
        //                 {
        //                     khnode2node[it.first].insert(node);
        //                 }
        //             }
        //         }
        //     }
        // }
        // fout.open("result/" + str + "/" + str + "-khnode2node.txt");
        // for (auto &it : khnode2node)
        // {
        //     for (auto &node : it.second)
        //         fout << it.first << ' ' << node << endl;
        // }
        // fout.close();
        cout << "*****************************************************" << endl;
    }
}