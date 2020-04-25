#pragma once
#include <vector>
#include <memory>
#include "element.h"
#include "basic_elements.h"

class sim;
class elem_meta:public element{
public:
    using elem_vec = std::vector<std::unique_ptr<element>>;
protected:
friend class sim;
friend class elem_file_saver;
    elem_vec elements;
public:
    elem_meta(const std::string &name)
        :element(name),
        nameable(name)
    {}

    virtual bool get_processed()const override{
        for(auto &el:elements){
            if(!el->get_processed()){
                return false;
            }
        }
        return true;
    }

    virtual void reset_processed()override{
        for(auto &el:elements){
            el->reset_processed();
        }
    }

    void process()override{
        for(auto &el:elements){
            el->process();
        }
    }

    const std::unique_ptr<element>& find_element(const size_t &id)const{
        for(auto it = elements.cbegin(); it != elements.cend(); it++){
            auto el = it.base()->get();
            if(el->get_id() == id){
                return *it;
            }
            auto meta_cast = dynamic_cast<elem_meta*>(el);
            if(meta_cast){
                try{
                    auto &recursive_result = meta_cast->find_element(id);
                    return recursive_result;
                }catch(std::out_of_range &e){
                }
            }
        }
        throw std::out_of_range("no element with id "+std::to_string(id));
        return *elements.begin();
    }

    std::unique_ptr<element>& find_element(const size_t &id){
        auto c_this = const_cast<const elem_meta*>(this);
        auto &c_el = c_this->find_element(id);
        return const_cast<std::unique_ptr<element>&>(c_el);
    }

    virtual std::shared_ptr<const gate> find_gate(const size_t &id)const override{
        auto outer_gt = element::find_gate(id);
        if(outer_gt){
            return outer_gt;
        }
        for(auto &el:elements){
            auto el_ptr = el.get();
            auto gt = el_ptr->find_gate(id);
            if(gt){
                return gt;
            }
            auto in_cast = dynamic_cast<elem_in*>(el_ptr);
            if(in_cast && in_cast->get_id() == id){
                return in_cast->get_out(0);
            }
            auto out_cast = dynamic_cast<elem_out*>(el_ptr);
            if(out_cast && out_cast->get_id() == id){
                return out_cast->get_in(0);
            }
        }
        return nullptr;
    }
    virtual std::shared_ptr<gate> find_gate(const size_t &id)override{
        const auto c_this = const_cast<const elem_meta*>(this);
        auto c_gt = c_this->find_gate(id);
        return std::const_pointer_cast<gate>(c_gt);
    }
    auto get_begin()        { return elements.begin(); }
    auto get_begin()const   { return elements.cbegin(); }
    auto get_rbegin()       { return elements.rbegin(); } 
    auto get_rbegin()const  { return elements.crbegin(); } 
    auto get_end()          { return elements.end(); } 
    auto get_end()const     { return elements.cend(); } 
    auto get_rend()         { return elements.rend(); } 
    auto get_rend()const    { return elements.crend(); } 
    const auto& get_sub_elements()const{ return elements; } 

    auto& emplace_back(std::unique_ptr<element> &&el){
        auto el_ptr = el.get();
        auto gate_in = dynamic_cast<elem_in*>(el_ptr);
        auto gate_out = dynamic_cast<elem_out*>(el_ptr);
        if(gate_in){
            auto outer = gate_in->gt_outer;
            element::emplace_back(outer);
        }else if(gate_out){
            auto outer = gate_out->gt_outer;
            element::emplace_back(outer);
        }
        return elements.emplace_back(std::move(el));
    }

    void erase(const elem_vec::const_iterator &it){
        elements.erase(it);
    }

    friend bool operator==(const elem_meta &lhs, const elem_meta &rhs){
        const element &lhs_el(lhs);
        const element &rhs_el(rhs);
        bool els_eq = std::equal(lhs.elements.begin(), lhs.elements.end(), rhs.elements.begin(),
            [](auto &ptr1, auto &ptr2){
                return (ptr1 && ptr2) && (*ptr1 == *ptr2);
            });
        return lhs_el == rhs_el &&
            els_eq;
    }
    friend bool operator!=(const elem_meta &lhs, const elem_meta &rhs){
        return !(lhs == rhs);
    }
};
