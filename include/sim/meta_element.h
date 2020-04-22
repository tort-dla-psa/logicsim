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

    virtual const std::unique_ptr<element>& find_element(const size_t id)const{
        auto it = std::find_if(elements.begin(), elements.end(), 
            [&id](auto &el){
                return el->get_id() == id;
            });
        return *it;
    }

    virtual std::unique_ptr<element>& find_element(const size_t id){
        auto it = std::find_if(elements.begin(), elements.end(), 
            [&id](auto &el){
                return el->get_id() == id;
            });
        return *it;
    }

    virtual std::shared_ptr<const gate> find_gate(const size_t &id)const override{
        for(auto &el:elements){
            auto el_ptr = el.get();
            auto gt = el_ptr->find_gate(id);
            if(gt){
                return gt;
            }
            auto in_cast = dynamic_cast<elem_in*>(el_ptr);
            if(in_cast && in_cast->get_id() == id){
                return in_cast->get_in(0);
            }
            auto out_cast = dynamic_cast<elem_out*>(el_ptr);
            if(out_cast && out_cast->get_id() == id){
                return out_cast->get_out(0);
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
        if(gate_in){
            this->ins.emplace_back(gate_in->get_in(0));
        }
        auto gate_out = dynamic_cast<elem_out*>(el_ptr);
        if(gate_out){
            this->outs.emplace_back(gate_out->get_out(0));
        }
        return elements.emplace_back(std::move(el));
    }

    void erase(const elem_vec::const_iterator &it){
        elements.erase(it);
    }
};
