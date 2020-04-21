#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "meta_element.h"
#include "element.h"
#include "basic_elements.h"

class sim{
    using elem_vec = std::vector<std::shared_ptr<element>>;
private:
    std::unique_ptr<elem_meta> root;
    elem_vec::const_iterator find_by_id(const elem_vec &elements, const size_t &id)const{
        for(auto it = elements.cbegin(); it != elements.cend(); it++){
            auto el = *it;
            if(el->get_id() == id){
                return it;
            }
            auto meta_cast = std::dynamic_pointer_cast<elem_meta>(el);
            if(meta_cast){
                return find_by_id(meta_cast->elements, id);
            }
        }
        auto mes = "there's no element with id "+std::to_string(id)+
            " in simulation";
        throw std::runtime_error(mes);
    }
    auto find_by_id(const size_t &id)const{
        auto &elements = root->elements;
        return find_by_id(elements, id);
    }
    std::shared_ptr<gate> find_gate_by_id(const elem_vec &elements, const size_t &id)const{
        for(auto it = elements.cbegin(); it != elements.cend(); it++){
            auto el = *it;
            auto meta_cast = std::dynamic_pointer_cast<elem_meta>(el);
            if(meta_cast){
                return find_gate_by_id(meta_cast->elements, id);
            }
            for(auto &gt_:el->access_gates()){
                if(gt_->get_id() == id){
                    return gt_;
                }
            }
            auto in_cast = std::dynamic_pointer_cast<elem_in>(el);
            if(in_cast && in_cast->get_in_id() == id){
                return in_cast;
            }
            auto out_cast = std::dynamic_pointer_cast<elem_out>(el);
            if(out_cast && out_cast->get_out_id() == id){
                return out_cast;
            }
        }
        auto mes = "there's no gate with id "+std::to_string(id)+
            " in simulation";
        throw std::runtime_error(mes);
    }
    auto find_gate_by_id(const size_t &id)const{
        auto &elements = root->elements;
        return find_gate_by_id(elements, id);
    }
    template<class T>
    auto find_type_by_id(const elem_vec &elements, const size_t &id)const{
        auto el = *find_by_id(elements, id);
        auto out = std::dynamic_pointer_cast<T>(el);
        if(!out){
            auto mes = "element with id "+std::to_string(id)
                +" has not requested type";
            throw std::runtime_error(mes);
        }
        return out;
    }
    template<class T>
    auto find_type_by_id(const size_t &id)const{
        auto &elements = root->elements;
        return find_type_by_id<T>(elements, id);
    }
public:
    sim(){
        root = std::make_unique<elem_meta>("root");
    }
    auto get_sub_elements(const size_t &id)const{
        const auto meta = find_type_by_id<elem_meta>(id);
        auto elems = meta->get_sub_elements();
        std::vector<size_t> ids;
        std::transform(elems.begin(), elems.end(), std::back_inserter(ids),
            [](const auto el){
                return el->get_id();
        });
        return ids;
    }
    auto get_element(const size_t &id)const{
        return *find_by_id(id);
    }
    void delete_element(const size_t &id){
        auto el_it = find_by_id(id);
        auto &elements = root->elements;
        elements.erase(el_it);
    }
    void add_element_into_meta(const size_t &id, std::shared_ptr<element> el){
        if(id == 0){
            root->add_element(std::move(el));
        }else{
            auto meta = find_type_by_id<elem_meta>(id);
            meta->add_element(std::move(el));
        }
    }
    void add_element(std::shared_ptr<element> el){
        auto &elements = root->elements;
        elements.emplace_back(el);
    }
    template<class T, typename ... Args>
    auto create_element(Args... args){
        return std::make_shared<T>(args...);
    }
    void start(){
        root->process();
    }
    void set_out_value(const size_t &id, const std::vector<bool> &val){
        auto el = find_type_by_id<elem_out>(id);
        el->set_values(val);
    }
    auto get_gate_value(const size_t &id, const size_t &place)const{
        auto el = *find_by_id(id);
        auto cast = std::dynamic_pointer_cast<gate>(el);
        if(cast){
            if(place == 0){
                return cast->get_values();
            }else{
                auto mes = "asked value of gate number "+std::to_string(place)+
                    ", which is illegal";
                throw std::runtime_error(mes);
            }
        }
        return el->get_in(place)->get_values();
    }
    void set_gate_width(const size_t &id, const size_t &width){
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
    auto get_gate_width(const elem_vec &elems, const size_t &id)const{
        auto gt = find_gate_by_id(elems, id);
        if(gt){
            return gt->get_width();
        }
        auto mes = "ID "+std::to_string(id)+" is not a gate, getting bit_width is illegal";
        throw std::runtime_error(mes);
    }
    auto get_gate_width(const size_t &id_parent, const size_t &id)const{
        auto parent = *find_by_id(id_parent);
        if(id_parent == id){
            return std::dynamic_pointer_cast<gate>(parent)->get_width();
        }
        return get_gate_width({parent}, id);
    }
    auto get_gate_width(const size_t &id)const{
        auto &elements = root->elements;
        return get_gate_width(elements, id);
    }
    void connect_gates(const size_t &id1, const size_t &id2){
        auto &elements = root->elements;
        std::shared_ptr<gate> gt1, gt2;
        for(auto it = elements.begin(); it < elements.end(); it++){
            auto &gates = (*it)->access_gates();
            for(auto &gt:gates){
                if(gt->get_id() == id1 && !gt1){
                    gt1 = gt;
                    continue;
                }
                if(gt->get_id() == id2 && !gt2){
                    gt2 = gt;
                    break;
                }
            }
            auto cast = std::dynamic_pointer_cast<gate>(*it);
            if(!cast){
                continue;
            }
            if(!gt1 && cast->get_id() == id1){
                gt1 = cast;
                continue;
            }
            if(!gt2 && cast->get_id() == id2){
                gt2 = cast;
                continue;
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