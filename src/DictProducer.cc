#include "DictProducer.h"
#include "SplitTool.h"
#include <ctype.h>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

DictProducer::DictProducer(const string& dir, SplitTool* splitTool)
    : _dir(dir)
    , _files()
    , _dict()
      , _splitTool(splitTool)
{
    _files.reserve(10);
    getFiles();
}

void DictProducer::buildCnDict() {
    for (auto& filename : _files) {
        ifstream ifs(filename);
        if (!ifs) {
            cerr << filename << "open failed" << endl; 
        }
        string sentence;
        vector<string> words;
        while (getline(ifs, sentence)) {
            words = _splitTool->cut(sentence);
            for (auto& word : words) {//用来删除非汉字
                word.erase( 
                           remove_if ( word.begin(), word.end(), static_cast<int(*)(int)>(&ispunct)  ), 
                           word.end()
                          );
                word.erase( 
                           remove_if ( word.begin(), word.end(), static_cast<int(*)(int)>(&isspace)  ), 
                           word.end()
                          );
                word.erase( 
                           remove_if ( word.begin(), word.end(), static_cast<int(*)(int)>(&isgraph)  ), 
                           word.end()
                          );
                word.erase( 
                           remove_if ( word.begin(), word.end(), static_cast<int(*)(int)>(&isalnum)  ), 
                           word.end()
                          );
                pushDict(word);
            }
        }

        ifs.close();
    }


}

void DictProducer::storeDict(const char* filepath) {
    ofstream ofs(filepath, ios::in | ios::trunc);
    if (!ofs) {
        cerr << filepath << "open failed" << endl; 
        return;
    }

    map<string, int>::iterator iter;
    for (iter = _dict.begin(); iter != _dict.end(); ++iter) {

        /* cout<<iter->first<<endl; */
        ofs << left << setw(20) << iter->first
            << right << setw(10) << iter->second << endl;
    }

    ofs.close();
}

void DictProducer::showFiles() const {
    cout << "yuliaoPath = " << _dir << endl;
}

void DictProducer::showDict() const {
    cout << "" << endl;
}

void DictProducer::getFiles() {
    char yuliaoPath[1024] = {0};
    struct dirent* ptr;
    DIR* dir = opendir(_dir.c_str());
    while (0 != (ptr = readdir(dir))) {
        if (0 == strcmp(ptr->d_name, ".") || 0 == strcmp(ptr->d_name, "..")) 
            continue;
        sprintf(yuliaoPath, "%s%c%s", _dir.c_str(), '/', ptr->d_name);
        _files.push_back(yuliaoPath);
    }
    for (auto file : _files) {
        cout << file << endl;
    }
}
string DictProducer::getFiles(char *filename)
{
    return _dir + string("/") + string(filename);

}

void DictProducer::pushDict(const string& word) {
    if (string() != word) 
        ++_dict[word];
}
//中文分字算法
void splitWord(const string & word, vector<string> & characters)
{
    int num = word.size();
    int i = 0;
    while(i < num)
    {
        int size = 1;
        if(word[i] & 0x80)//0x80 三字节代表一个中文汉字
        {
            char temp = word[i];
            temp <<= 1;
            do{
                temp <<= 1;
                ++size;

            }while(temp & 0x80);

        }
        string subWord;
        subWord = word.substr(i, size);
        characters.push_back(subWord);
        i += size;

    }

}
void DictProducer::buildicnIndex(){

    ifstream ifs("../data/cn_dict.dat");
    if (!ifs) {
        cerr << "dict.dat open failed" << endl;
        return;
    }
    int lineNum = 1;
    string line;
    string word;
    while (getline(ifs, line)) {
        istringstream iss(line);
        iss >> word;        //只输入第一部分的单词
        vector<string> chs;
        splitWord(word, chs);
        for (auto ch : chs) {
            _index[ch].insert(lineNum);
            cout << "ch = " << ch
                << " lineNum = " << lineNum << endl;

        }
        ++lineNum;

    }
}
void DictProducer::storeIndex(const string & filepath) const
{

    ofstream ofs(filepath);
    if(!ofs.good())
    {
        cerr << "idx.txt open failed" <<endl;

    }
    for(auto& it1 : _index)
    {
        ofs << it1.first <<" ";
        for(auto& it2 : it1.second)
        {
            ofs << it2 <<" ";

        }
        ofs << endl;

    }
}
DictProducer::DictProducer(const string & dir)
    :_dir(dir){
        DIR *pdir = opendir(dir.c_str());
        struct dirent *pdirent;
        while((pdirent = readdir(pdir)))
        {
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
                continue;
            _files.push_back(getFiles(pdirent->d_name));

        }
        closedir(pdir);
    }

void DictProducer::buildEnDict(const string &stopWordsPath)
{
    string line;
    string word;
    for(auto &file : _files)
    {
        ifstream ifs(file);
        if(!ifs)
        {
            cerr << "ifs open " << file << " error!" << endl;
            return;

        }

        while(getline(ifs, line))
        {
            /*标点符号转换为空格*/
            for(size_t i = 0; i < line.size(); ++i)
            {
                if(!isalpha(line[i]) && line[i] != ' ')
                    line[i] = ' ';
                else if(isupper(line[i]))
                    line[i] += 32;

            }

            istringstream iss(line);
            while(iss >> word)
            {
                std::pair<map<string, int>::iterator, bool> ret = _dict.insert(std::pair<string, int>(word, 1));
                if(!ret.second)
                {
                    ++_dict[word];

                }

            }

        }
        ifs.close();

    }


    ifstream ifs(stopWordsPath);
    if(!ifs)
    {
        cerr << "ifs open error!" << endl;
        return;

    }
    while(ifs >> word)
    {
        auto it = _dict.find(word);
        if(it != _dict.end())
        {
            _dict.erase(it);

        }

    }
    ifs.close();

}


void DictProducer::buildienIndex()
{
    int idx = 0;
    for(auto &elem : _dict)
    {
        for(size_t i = 0; i < elem.first.size(); ++i)
        {
            string tmp;
            tmp.push_back(elem.first[i]);
            _index[tmp].insert(idx);

        }
        ++idx;

    }

}
