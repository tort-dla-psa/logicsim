#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "gate_in.h"
#include "gate.h"

class gate_out:public gate{
    std::vector<std::shared_ptr<gate_in>> ins;
public:
    gate_out(const std::string &name, size_t width=1)
        :gate(name, width)
    {}
    void pass_value()const{
        for(auto &in:ins){
            in->set_values(this->get_values());
        }
    }
    void pass_value(const std::vector<bool> &val){
        set_values(val);
        pass_value();
    }
    void tie_input(std::shared_ptr<gate_in> in){
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
};
