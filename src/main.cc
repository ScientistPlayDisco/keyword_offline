#include "DictProducer.h"
#include "SplitToolCppJieba.h"
#include "Configuration.h"
SplitToolCppJieba SplitToolCppJieba::myInstance;

void test() {
    Configuration conf("../conf/dic_conf");
    map<string,string> path = conf.getConfigMap();

    DictProducer endict(path["enConfig"]);
    endict.buildEnDict(path["enStop"]);
    endict.storeDict("../data/en_dict.dat");
    endict.buildienIndex();
    endict.storeIndex("../data/en_idx.dat");


    DictProducer dict(path["cnConfig"], SplitToolCppJieba::getInstance());
    dict.buildCnDict();
    dict.storeDict("../data/cn_dict.dat");
    dict.buildicnIndex();
    cout<<1<<endl;
    dict.storeIndex("../data/cn_idx.dat");
    std::cout<<"store"<<std::endl;

}

int main()
{
    test();
    return 0;
}
