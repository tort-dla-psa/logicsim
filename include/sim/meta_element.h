#pragma once
#include <vector>
#include <memory>
#include "element.h"

class meta_element:public element{
protected:
    std::vector<std::unique_ptr<element>> elements;
public:
    meta_element(const std::string &name)
        :element(name)
    {}
    virtual ~meta_element(){}

    void process(){
        for(auto &el:elements){
            el->process();
        }
    }
};
