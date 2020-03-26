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
    void connect_gates(size_t id1, size_t id2){
        auto it1 = elements.end();
        auto it2 = elements.end();
        for(auto it = elements.begin(); it < elements.end(); it++){
            if(it->get()->get_id() == id1 && 
                it1 == elements.end())
            {
                it1 = it;
                continue;
            }
            if(it->get()->get_id() == id2 && 
                it2 == elements.end())
            {
                it2 = it;
                break;
            }
        }
        if(it1 == elements.end()){
            auto mes = "no elements with id "+std::to_string(id1);
            throw std::runtime_error(mes);
        }
        if(it2 == elements.end()){
            auto mes = "no elements with id "+std::to_string(id2);
            throw std::runtime_error(mes);
        }
    }
};