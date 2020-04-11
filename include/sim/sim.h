#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "meta_element.h"
#include "element.h"
#include "basic_elements.h"

class sim{
private:
    std::vector<std::shared_ptr<element>> elements;
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
    template<class T>
    auto find_type_by_id(size_t id){
        auto el = *find_by_id(id);
        auto out = std::dynamic_pointer_cast<T>(el);
        if(!out){
            auto mes = "element with id "+std::to_string(id)
                +" has not requested type";
            throw std::runtime_error(mes);
        }
        return out;
    }
public:
    std::shared_ptr<element>& get_element(size_t id){
        return *find_by_id(id);
    }
    void delete_element(size_t id){
        elements.erase(find_by_id(id));
    }
    void add_element(std::unique_ptr<element> &&el){
        elements.emplace_back(std::move(el));
    }
    template<class T, typename ... Args>
    size_t create_element(Args... args){
        const auto el = std::make_shared<T>(args...);
        elements.emplace_back(el);
        return el->T::get_id();
    }
    void start(){
        for(auto &el:elements){
            el->process();
        }
    }
    void set_out_value(size_t id, const std::vector<bool> &val){
        auto el = find_type_by_id<elem_out>(id);
        el->set_values(val);
    }
    auto get_out_value(size_t id){
        auto el = find_type_by_id<elem_out>(id);
        return el->get_values();
    }
    size_t get_out_width(size_t id){
        auto el = find_type_by_id<elem_out>(id);
        return el->get_width();
    }
    auto get_in_value(size_t id, size_t place){
        auto el = *find_by_id(id);
        auto in_cast = std::dynamic_pointer_cast<gate_in>(el);
        if(in_cast){
            if(place == 0){
                return in_cast->get_values();
            }else{
                auto mes = "asked value of input number "+std::to_string(place)+
                    ", which is illegal";
                throw std::runtime_error(mes);
            }
        }
        return el->get_in(place)->get_values();
    }
    auto get_out_value(size_t id, size_t place){
        auto el = *find_by_id(id);
        auto out_cast = std::dynamic_pointer_cast<gate_out>(el);
        if(out_cast){
            if(place == 0){
                return out_cast->get_values();
            }else{
                auto mes = "asked value of output number "+std::to_string(place)+
                    ", which is illegal";
                throw std::runtime_error(mes);
            }
        }
        return el->get_out(place)->get_values();
    }
    void set_gate_width(size_t id, size_t width){
        auto el = *find_by_id(id);
        auto out_cast = std::dynamic_pointer_cast<gate_out>(el);
        if(out_cast){
            out_cast->set_width(width);
            return;
        }
        auto in_cast = std::dynamic_pointer_cast<gate_in>(el);
        if(in_cast){
            in_cast->set_width(width);
            return;
        }
        auto mes = "ID "+std::to_string(id)+" is not a gate, setting bit_width is illegal";
        throw std::runtime_error(mes);
    }
    auto get_gate_width(size_t id){
        auto el = *find_by_id(id);
        auto cast = std::dynamic_pointer_cast<gate>(el);
        if(cast){
            return cast->get_width();
        }
        auto mes = "ID "+std::to_string(id)+" is not a gate, getting bit_width is illegal";
        throw std::runtime_error(mes);
    }
    void connect_gates(size_t id1, size_t id2){
        std::shared_ptr<gate> gt1, gt2;
        for(auto it = elements.begin(); it < elements.end(); it++){
            auto &gates = (*it)->access_gates();
            for(auto gt = gates.begin(); gt != gates.end(); gt++){
                if(gt->get()->get_id() == id1 && !gt1){
                    gt1 = *gt;
                    continue;
                }
                if(gt->get()->get_id() == id2 && !gt2){
                    gt2 = *gt;
                    break;
                }
            }
            {
                auto cast = std::dynamic_pointer_cast<gate>(*it);
                if(!gt1 && cast && cast->get_id() == id1){
                    gt1 = cast;
                    continue;
                }
            }
            {
                auto cast = std::dynamic_pointer_cast<gate>(*it);
                if(!gt2 && cast && cast->get_id() == id2){
                    gt2 = cast;
                    continue;
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