#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "gate_in.h"
#include "gate.h"
#include "helpers.h"

class gate_out:public gate{
public:
    using ins_vec = std::vector<std::shared_ptr<gate_in>>;
private:
    friend class elem_file_saver;
    ins_vec ins;
public:
    gate_out(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
    ~gate_out(){}

    void pass_value()const{
        auto &val = this->get_values();
        for(auto &in:ins){
            in->set_values(val);
        }
    }
    void pass_value(const std::vector<bool> &val){
        set_values(val);
        pass_value();
    }
    void tie_input(const std::shared_ptr<gate_in> &in){
        if(in->get_width() != this->get_width()){
            auto mes = "attempt to tie input "+in->get_name()+
                " of width "+std::to_string(in->get_width())+
                "to an output "+get_name()+" of width "+std::to_string(get_width());
            throw std::runtime_error(mes);
        }
        this->ins.emplace_back(in);
    }
    void untie_input(const std::shared_ptr<gate_in> &in){
        auto it = std::find(ins.begin(), ins.end(), in);
        if(it == ins.end()){
            auto mes = "attempt to untie input "+in->get_name()+
                "from an output "+get_name()+", but they are not tied";
            throw std::runtime_error(mes);
        }
        ins.erase(it);
    }
    bool tied(const std::shared_ptr<gate_in> &in){
        auto it = std::find(ins.begin(), ins.end(), in);
        return !(it == ins.end());
    }

    auto get_tied_begin()       { return ins.begin(); }
    auto get_tied_begin()const  { return ins.begin(); }
    auto get_tied_cbegin()const { return ins.begin(); }
    auto get_tied_rbegin()      { return ins.rbegin(); }
    auto get_tied_rbegin()const { return ins.rbegin(); }
    auto get_tied_crbegin()const{ return ins.rbegin(); }
    auto get_tied_end()         { return ins.end(); }
    auto get_tied_end()const    { return ins.end(); }
    auto get_tied_cend()const   { return ins.end(); }
    auto get_tied_rend()        { return ins.rend(); }
    auto get_tied_rend()const   { return ins.rend(); }
    auto get_tied_crend()const  { return ins.rend(); }
    auto& get_tied()const       { return ins; }
    auto& get_tied()            { return ins; }

    friend bool operator==(const gate_out &lhs, const gate_out &rhs){
        const nameable &lhs_n(lhs);
        const nameable &rhs_n(rhs);
        const gate &lhs_g(lhs);
        const gate &rhs_g(rhs);
        bool ins_eq = std::equal(lhs.ins.begin(), lhs.ins.end(), rhs.ins.begin(),
            [](auto ptr1, auto ptr2){
                return (ptr1 && ptr2) && (*ptr1 == *ptr2);
            });
        return lhs_n == rhs_n &&
            lhs_g == rhs_g &&
            ins_eq;
    }
    friend bool operator!=(const gate_out &lhs, const gate_out &rhs){
        return !(lhs == rhs);
    }
};
