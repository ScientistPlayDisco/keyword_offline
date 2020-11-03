#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
using std::set;
using std::unordered_map;
using std::string;
using std::vector;
using std::map;

class SplitTool;

class DictProducer
{
public:
    DictProducer(const string& dir, SplitTool* splitTool);
    DictProducer(const string& dir);
    void buildCnDict();                     //创建中文字典
    
    void buildEnDict(const string &stopWordsPath);
    void storeDict(const char* filepath);   //将词典写入文件
    void showFiles() const;                 //查看文件路径，作为测试用
    void showDict() const;                  //查看词典，作为测试用
    void buildicnIndex();
    void buildienIndex();
    void storeIndex(const string & filepath) const;
private:
    void getFiles();               //获取文件的绝对路径
    string getFiles(char * filename);
    void pushDict(const string& word);      //存储某个单词

private:
    string _dir;
    vector<string> _files;
    map<string, int> _dict;
    SplitTool* _splitTool;
    unordered_map<string,set<int>> _index;
};


#endif
