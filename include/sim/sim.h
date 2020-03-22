#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "meta_element.h"
#include "element.h"
#include "basic_elements.h"

class sim{
private:
    std::vector<std::unique_ptr<element>> elements;
    auto find_by_id(size_t id){
        auto el = std::find_if(elements.begin(), elements.end(),
            [&id](const auto &el){
                return id == el->get_id();
            }
        );
        if(el == elements.end()){
            auto mes = "there's no element with id "+std::to_string(id)+
                "in simulation";
            throw std::runtime_error(mes);
        }
        return el;
    }
public:
    std::unique_ptr<element>& get_element(size_t id){
        return *find_by_id(id);
    }
    void delete_element(size_t id){
        elements.erase(find_by_id(id));
    }
    void add_element(std::unique_ptr<element> &&el){
        elements.emplace_back(std::move(el));
    }
    void start(){
        for(auto &el:elements){
            el->process();
        }
    }
};