#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;
typedef pair<int, int> PII;
/**
 * @brief 从文件中读取khcore，并统计khcore的数量，再写回文件
 * 
 * @param filename 文件名
 * @param num khcore的数量统计
 */
void comput(string &filename, map<PII, int> &num)
{
    num.clear();
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
        num[make_pair(b, c)]++;
    }
}

int main()
{
    string file = "S_MiAc S_DrAb S_TrCl  S_WaTr  T_CoMH  T_ThAU  T_ThMS T_congress-bills T_DBLP S_MaAn";
    string filepath = "./result/";
    istringstream ss(file);
    string str;
    while (ss >> str)
    {
        string filename = filepath + str + "/" + str + "-khcore.txt";
        map<PII, int> num;
        comput(filename, num);
        ofstream fout(filepath + str + "/" + str + "-khcorenum.txt");
        vector<int> x;
        vector<int> y;
        vector<int> z;
        for (auto &it : num)
        {
            x.push_back(it.first.first);
            y.push_back(it.first.second);
            z.push_back(it.second);
        }
        for (int i = 0; i < x.size(); i++)
            fout << x[i] << ' ';
        fout << endl;
        for (int i = 0; i < y.size(); i++)
            fout << y[i] << ' ';
        fout << endl;
        for (int i = 0; i < z.size(); i++)
            fout << z[i] << ' ';
        fout << endl;
        fout.close();
        cout << str << "已经完成！" << endl;
    }
}