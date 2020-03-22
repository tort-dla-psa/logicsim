#pragma once
#include <vector>
#include <memory>
#include "gate_in.h"
#include "gate_out.h"
#include "nameable.h"

class element:public nameable{
protected:
    std::vector<std::shared_ptr<gate_in>> ins;
    std::vector<std::shared_ptr<gate_out>> outs;
public:
    element(const std::string &name)
        :nameable(name)
    {}
    virtual ~element(){}
    virtual void process()=0;

    size_t get_ins()const{
        return ins.size();
    }
    size_t get_outs()const{
        return outs.size();
    }
    auto get_in(size_t place)const{
        return ins.at(place);
    }
    auto get_out(size_t place)const{
        return outs.at(place);
    }
    void add_in(const std::shared_ptr<gate_in> in){
        ins.emplace_back(in);
    }
    void add_in(const std::shared_ptr<gate_in> in, size_t place){
        if(place > ins.size()){
            auto mes = "attempt to add input in place "+std::to_string(place)+
                "to element "+get_name()+", which has "+std::to_string(get_ins())+
                "inputs";
            throw std::runtime_error(mes);
        }
        ins.emplace(ins.begin()+place, in);
    }
    void add_out(const std::shared_ptr<gate_out> out){
        outs.emplace_back(out);
    }
    void add_out(const std::shared_ptr<gate_out> out, size_t place){
        if(place > outs.size()){
            auto mes = "attempt to add output in place "+std::to_string(place)+
                "to element "+get_name()+", which has "+std::to_string(get_ins())+
                "outputs";
            throw std::runtime_error(mes);
        }
        outs.emplace(outs.begin()+place, out);
    }
};
