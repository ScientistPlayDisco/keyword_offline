// Authors: Handling
// Created by 60263 on 2020/11/2.
// This is 

#ifndef SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_H_
#define SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_H_
#include "../cppjieba/include/cppjieba/Jieba.hpp"
#include "SplitTool.h"
class SplitToolCppJieba : public SplitTool{
 private:
  cppjieba::Jieba _jieba;
  SplitToolCppJieba();
  ~SplitToolCppJieba() = default;
  vector<string> cut(const string &sentence) override;
  static SplitToolCppJieba myInstance;
 public:
  static SplitToolCppJieba* getInstance(){
    return &myInstance;
  }

};

#endif //SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_H_
