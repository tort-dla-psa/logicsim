#pragma once
#include <vector>
#include <memory>
#include "element.h"
#include "basic_elements.h"

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
        auto gate_in = std::dynamic_pointer_cast<elem_in>(el);
        if(gate_in){
            this->ins.emplace_back(gate_in);
        }
        auto gate_out = std::dynamic_pointer_cast<elem_out>(el);
        if(gate_out){
            this->outs.emplace_back(gate_out);
        }
        elements.emplace_back(el);
    }

    void del_element(std::shared_ptr<element> el){
        auto it = std::find(elements.begin(), elements.end(), el);
        if(it != elements.end()){
            elements.erase(it);
        }
    }
};
