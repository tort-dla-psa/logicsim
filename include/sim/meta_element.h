#pragma once
#include <vector>
#include <memory>
#include "element.h"

class elem_meta:public element{
protected:
    std::vector<std::unique_ptr<element>> elements;
public:
    elem_meta(const std::string &name)
        :element(name),
        nameable(name)
    {}

    void process(){
        for(auto &el:elements){
            el->process();
        }
    }

    auto get_sub_elements()const{
        std::vector<std::reference_wrapper<const element>> result;
        result.reserve(elements.size());
        std::transform(elements.begin(), elements.end(), std::back_inserter(result),
            [](const auto &el){
                return std::cref(*el);
            });
        return result;
    }
    auto get_sub_elements(){
        std::vector<std::reference_wrapper<element>> result;
        result.reserve(elements.size());
        std::transform(elements.begin(), elements.end(), std::back_inserter(result),
            [](const auto &el){
                return std::ref(*el);
            });
        return result;
    }
};
