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
    ins_vec ins;
public:
    gate_out(const std::string &name, const size_t &width=1)
        :gate(name, width),
        nameable(name)
    {}
    ~gate_out(){}

    void pass_value()const{
        auto &val = this->get_values();
        for(auto &in:ins){
            log("passing value "+sim_helpers::to_str(val)+
                " to gate id:"+std::to_string(in->get_id()));
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
    auto& get_tied()const{
        return ins;
    }
};
