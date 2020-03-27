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
        std::shared_ptr<gate> gt1, gt2;
        for(auto it = elements.begin(); it < elements.end(); it++){
            auto &gates_in = (*it)->access_ins();
            for(auto gt_in = gates_in.begin(); gt_in != gates_in.end(); gt_in++){
                if(gt_in->get()->get_id() == id1 && !gt1){
                    gt1 = *gt_in;
                    continue;
                }
                if(gt_in->get()->get_id() == id2 && !gt2){
                    gt2 = *gt_in;
                    break;
                }
            }
            auto &gates_out = (*it)->access_ins();
            for(auto gt_out = gates_out.begin(); gt_out != gates_out.end(); gt_out++){
                if(gt_out->get()->get_id() == id1 && !gt1){
                    gt1 = *gt_out;
                    continue;
                }
                if(gt_out->get()->get_id() == id2 && !gt2){
                    gt2 = *gt_out;
                    break;
                }
            }
        }
        if(!gt1){
            auto mes = "no elements with id "+std::to_string(id1);
            throw std::runtime_error(mes);
        }
        if(!gt2){
            auto mes = "no elements with id "+std::to_string(id2);
            throw std::runtime_error(mes);
        }
        auto cast_in_1 = std::dynamic_pointer_cast<gate_in>(gt1);
        auto cast_out_2 = std::dynamic_pointer_cast<gate_out>(gt2);
        if(cast_in_1 && cast_out_2){
            cast_out_2->tie_input(cast_in_1);
            return;
        }
        auto cast_out_1 = std::dynamic_pointer_cast<gate_out>(gt1);
        auto cast_in_2 = std::dynamic_pointer_cast<gate_in>(gt2);
        if(cast_out_1 && cast_in_2){
            cast_out_1->tie_input(cast_in_2);
            return;
        }
    }
};