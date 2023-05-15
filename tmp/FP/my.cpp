#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <fstream>


using namespace std;


int count_lines(const string& filename);
int main(int argc,char**argv)
{
    cout<<"input the file name:"<<endl;

    vector<string> files;
    files.push_back(argv[1]);

    int result = 0;

    for(const auto & file:files){
        result+=count_lines(file);
    }
    /*
    for(auto p = file.cbegin();p!=file.cend();++p){
        result += count_lines(*p);
    }
    */
    cout<<result<<"files\n";

    return 0;
}

int count_lines(const string& filename)
{
    ifstream in(filename);

    return count(istreambuf_iterator<char>(in),istreambuf_iterator<char>(),'\n');

}