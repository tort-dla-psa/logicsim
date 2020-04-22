#pragma once
#include <vector>
#include <memory>
#include "gate_in.h"
#include "gate_out.h"
#include "nameable.h"

class element:virtual public nameable{
public:
    using ins_vec = std::vector<std::shared_ptr<gate_in>>;
    using outs_vec = std::vector<std::shared_ptr<gate_out>>;
    using gates_vec = std::vector<std::shared_ptr<gate>>;
protected:
    std::vector<std::shared_ptr<gate_in>> ins;
    std::vector<std::shared_ptr<gate_out>> outs;
    std::vector<std::shared_ptr<gate>> gates;

    bool processed;
public:
    element(const std::string &name)
        :nameable(name)
    {
        reset_processed();
    }
    virtual ~element(){}
    virtual void process()=0;

    auto get_outs_begin()       { return outs.begin(); }
    auto get_outs_begin()const  { return outs.cbegin(); }
    auto get_outs_rbegin()      { return outs.rbegin(); }
    auto get_outs_rbegin()const { return outs.crbegin(); }
    auto get_outs_end()         { return outs.end(); }
    auto get_outs_cend()const   { return outs.cend(); }
    auto get_outs_rend()        { return outs.rend(); }
    auto get_outs_rend()const   { return outs.crend(); }
    auto get_ins_begin()        { return ins.begin(); }
    auto get_ins_begin()const   { return ins.cbegin(); }
    auto get_ins_rbegin()       { return ins.rbegin(); }
    auto get_ins_rbegin()const  { return ins.crbegin(); }
    auto get_ins_end()          { return ins.end(); }
    auto get_ins_cend()const    { return ins.cend(); }
    auto get_ins_rend()         { return ins.rend(); }
    auto get_ins_rend()const    { return ins.crend(); }
    auto get_gates_begin()      { return gates.begin(); }
    auto get_gates_begin()const { return gates.cbegin(); }
    auto get_gates_rbegin()     { return gates.rbegin(); }
    auto get_gates_rbegin()const{ return gates.crbegin(); }
    auto get_gates_end()        { return gates.end(); }
    auto get_gates_cend()const  { return gates.cend(); }
    auto get_gates_rend()       { return gates.rend(); }
    auto get_gates_rend()const  { return gates.crend(); }
    virtual gates_vec& get_gates()              { return gates; }
    virtual const gates_vec& get_gates()const   { return gates; }
    virtual ins_vec& get_ins()                  { return ins; }
    virtual const ins_vec& get_ins()const       { return ins; }
    virtual outs_vec& get_outs()                { return outs; }
    virtual const outs_vec& get_outs()const     { return outs; }
    virtual size_t get_ins_size()const          { return ins.size(); }
    virtual size_t get_outs_size()const         { return outs.size(); }
    virtual size_t get_gates_size()const        { return gates.size(); }

    virtual std::shared_ptr<const gate> find_gate(const size_t &id)const{
        auto c_it = std::find_if(gates.begin(), gates.end(),
            [&id](auto gate){
                return gate->get_id() == id;
            });
        if(c_it == gates.end()){
            return nullptr;
        }
        return *c_it;
    }
    virtual std::shared_ptr<gate> find_gate(const size_t &id){
        auto c_this = const_cast<const element*>(this);
        auto c_gt = c_this->find_gate(id);
        return std::const_pointer_cast<gate>(c_gt);
    }

    virtual bool get_processed()const{
        return processed;
    }
    virtual void reset_processed(){
        processed = false;
    }

    virtual std::shared_ptr<const gate_in> get_in(const size_t &place)const{
        return ins.at(place);
    }
    virtual std::shared_ptr<const gate_out> get_out(const size_t &place)const{
        return outs.at(place);
    }
    virtual std::shared_ptr<gate_in> get_in(const size_t &place){
        return ins.at(place);
    }
    virtual std::shared_ptr<gate_out> get_out(const size_t &place){
        return outs.at(place);
    }
    void emplace_back(const std::shared_ptr<gate_in> in){
        ins.emplace_back(in);
        gates.emplace_back(in);
    }
    void emplace_back(const std::shared_ptr<gate_out> out){
        outs.emplace_back(out);
        gates.emplace_back(out);
    }
    void insert(const ins_vec::const_iterator it, const std::shared_ptr<gate_in> &in){
        ins.emplace(it, in);
        gates.emplace_back(in);
    }
    void insert(const outs_vec::const_iterator it, const std::shared_ptr<gate_out> &out){
        outs.emplace(it, out);
        gates.emplace_back(out);
    }
    void insert(const std::shared_ptr<gate_in> in, size_t place){
        if(place > ins.size()){
            auto mes = "attempt to add input in place "+std::to_string(place)+
                "to element "+get_name()+", which has "+std::to_string(get_ins_size())+
                "inputs";
            throw std::runtime_error(mes);
        }
        insert(ins.begin()+place, in);
    }
    void insert(const std::shared_ptr<gate_out> out, size_t place){
        if(place > outs.size()){
            auto mes = "attempt to add output in place "+std::to_string(place)+
                "to element "+get_name()+", which has "+std::to_string(get_outs_size())+
                "outputs";
            throw std::runtime_error(mes);
        }
        insert(outs.begin()+place, out);
    }
    void erase(ins_vec::const_iterator it){
        auto el = *it;
        auto gates_it = std::find(gates.begin(), gates.end(), el);
        ins.erase(it);
        gates.erase(gates_it);
    }
    void erase(outs_vec::const_iterator it){
        auto el = *it;
        auto gates_it = std::find(gates.begin(), gates.end(), el);
        outs.erase(it);
        gates.erase(gates_it);
    }
};
