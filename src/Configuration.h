#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <iostream>
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
class Configuration
{
public:
    Configuration(const string& filepath);
    map<string, string>& getConfigMap();

private:
    string _filepath;
    map<string, string> _configMap;
};


Configuration::Configuration(const string& filepath){
    char absolute_path[128] = {0};
    realpath(filepath.c_str(), absolute_path);
    _filepath = absolute_path;

    cout << _filepath << endl;

    ifstream ifs(_filepath);
    string line;

    vector<string> words = {"enConfig", "cnConfig", "enStop", "cnStop"};
    for(int i = 0; i < 4; ++i)
    {
        getline(ifs, line);
        bzero(&absolute_path, sizeof(absolute_path));
        realpath(line.c_str(), absolute_path);
        _configMap[words[i]] = absolute_path;

    }

}
map<string,string>& Configuration::getConfigMap(){
    return _configMap;
}
#endif
