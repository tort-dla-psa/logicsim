#pragma once
#include <vector>
#include <memory>
#include "element.h"

class sim;
class elem_meta:public element{
protected:
friend class sim;
    std::vector<std::shared_ptr<element>> elements;
public:
    elem_meta(const std::string &name)
        :element(name),
        nameable(name)
    {}

    void process(){
        for(auto &el:elements){
            el->reset_processed();
        }
        for(auto &el:elements){
            el->process();
        }
    }

    auto get_sub_elements()const{
        return elements;
    }

    void add_element(std::shared_ptr<element> &&el){
        elements.emplace_back(std::move(el));
    }

    void del_element(std::shared_ptr<element> el){
        auto it = std::find(elements.begin(), elements.end(), el);
        if(it != elements.end()){
            elements.erase(it);
        }
    }
};
