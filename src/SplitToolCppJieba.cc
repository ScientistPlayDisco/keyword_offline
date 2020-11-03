// Authors: Handling
// Created by 60263 on 2020/11/2.
// This is
#ifndef SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_CPP_
#define SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_CPP_
#include "SplitToolCppJieba.h"
const char *const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "../cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";
vector<string> SplitToolCppJieba::cut(const string &sentence) {

  vector<string> words;
  /* _jieba.CutForSearch(sentence, words); */
  _jieba.extractor.Extract(sentence,words,1000);
  //std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

  return words;
}
SplitToolCppJieba::SplitToolCppJieba() : _jieba(DICT_PATH,
                                                HMM_PATH,
                                                USER_DICT_PATH,
                                                IDF_PATH,
                                                STOP_WORD_PATH) {
}

#endif //SEARCH_ENGINE_SRC_SPLITTOOLCPPJIEBA_CPP_
