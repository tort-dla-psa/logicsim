#pragma once
#include <vector>
#include <memory>
#include "gate_out.h"
#include "gate_in.h"
#include "nameable.h"
#include "iserializable.h"

class element:virtual public nameable, virtual public ISerializable{
public:
    using ins_vec = std::vector<std::shared_ptr<gate_in>>;
    using outs_vec = std::vector<std::shared_ptr<gate_out>>;
protected:
    friend class elem_file_saver;
    std::vector<std::shared_ptr<gate_in>> ins;
    std::vector<std::shared_ptr<gate_out>> outs;

    bool processed;
public:
    element(const std::string &name)
        :nameable(name),
        ISerializable()
    {
        reset_processed();
    }
    virtual ~element(){}
    virtual void process(){}

    auto get_outs_begin()       { return outs.begin(); }
    auto get_outs_begin()const  { return outs.cbegin(); }
    auto get_outs_rbegin()      { return outs.rbegin(); }
    auto get_outs_rbegin()const { return outs.crbegin(); }
    auto get_outs_end()         { return outs.end(); }
    auto get_outs_end()const    { return outs.end(); }
    auto get_outs_cend()const   { return outs.cend(); }
    auto get_outs_rend()        { return outs.rend(); }
    auto get_outs_rend()const   { return outs.crend(); }
    auto get_ins_begin()        { return ins.begin(); }
    auto get_ins_begin()const   { return ins.cbegin(); }
    auto get_ins_rbegin()       { return ins.rbegin(); }
    auto get_ins_rbegin()const  { return ins.crbegin(); }
    auto get_ins_end()          { return ins.end(); }
    auto get_ins_end()const     { return ins.end(); }
    auto get_ins_cend()const    { return ins.cend(); }
    auto get_ins_rend()         { return ins.rend(); }
    auto get_ins_rend()const    { return ins.crend(); }
    virtual ins_vec& get_ins()                  { return ins; }
    virtual const ins_vec& get_ins()const       { return ins; }
    virtual outs_vec& get_outs()                { return outs; }
    virtual const outs_vec& get_outs()const     { return outs; }
    virtual size_t get_ins_size()const          { return ins.size(); }
    virtual size_t get_outs_size()const         { return outs.size(); }

    virtual std::shared_ptr<const gate> find_gate(const size_t &id)const{
        auto predicate = [&id](auto gate){
            return gate->get_id() == id;
        };
        auto c_in_it = std::find_if(ins.cbegin(), ins.cend(), predicate);
        if(c_in_it != ins.cend()){
            return *c_in_it;
        }
        auto c_out_it = std::find_if(outs.cbegin(), outs.cend(), predicate);
        if(c_out_it != outs.cend()){
            return *c_out_it;
        }
        return nullptr;
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
    }
    void emplace_back(const std::shared_ptr<gate_out> out){
        outs.emplace_back(out);
    }
    void insert(const ins_vec::const_iterator it, const std::shared_ptr<gate_in> &in){
        ins.emplace(it, in);
    }
    void insert(const outs_vec::const_iterator it, const std::shared_ptr<gate_out> &out){
        outs.emplace(it, out);
    }
    void insert(const std::shared_ptr<gate_in> in, size_t place){
        if(place > ins.size()){
            auto mes = "attempt to add input in place "+std::to_string(place)+
                "to element "+name()+", which has "+std::to_string(get_ins_size())+
                "inputs";
            throw std::runtime_error(mes);
        }
        insert(ins.begin()+place, in);
    }
    void insert(const std::shared_ptr<gate_out> out, size_t place){
        if(place > outs.size()){
            auto mes = "attempt to add output in place "+std::to_string(place)+
                "to element "+name()+", which has "+std::to_string(get_outs_size())+
                "outputs";
            throw std::runtime_error(mes);
        }
        insert(outs.begin()+place, out);
    }
    void erase(ins_vec::const_iterator it){
        ins.erase(it);
    }
    void erase(outs_vec::const_iterator it){
        outs.erase(it);
    }

    friend bool operator==(const element &lhs, const element &rhs){
        const nameable &lhs_n(lhs);
        const nameable &rhs_n(rhs);
        bool ins_eq = std::equal(lhs.ins.begin(), lhs.ins.end(), rhs.ins.begin(),
            [](auto ptr1, auto ptr2){
                return (ptr1 && ptr2) && (*ptr1 == *ptr2);
            });
        bool outs_eq = std::equal(lhs.outs.begin(), lhs.outs.end(), rhs.outs.begin(),
            [](auto ptr1, auto ptr2){
                return (ptr1 && ptr2) && (*ptr1 == *ptr2);
            });
        return lhs_n == rhs_n &&
            ins_eq && outs_eq;
    }
    friend bool operator!=(const element &lhs, const element &rhs){
        return !(lhs == rhs);
    }

    virtual bool from_json(const nlohmann::json &j)override{
        nameable::m_name =  j.at("name");
        nameable::m_id = j.at("id");
        nameable::m_parent_id = j.at("parent_id");
        auto &ins = get_ins();
        auto &outs = get_outs();
        ins.clear();
        outs.clear();
        bool status = true;
        for(auto &j_obj:j.at("ins")){
            auto gt_tmp = std::make_shared<gate_in>();
            status &= gt_tmp->from_json(j_obj);
            ins.emplace_back(gt_tmp);
        }
        for(auto &j_obj:j.at("outs")){
            auto gt_tmp = std::make_shared<gate_out>();
            status &= gt_tmp->from_json(j_obj);
            outs.emplace_back(gt_tmp);
        }
        return status;
    }

    virtual void to_json(nlohmann::json &j)const override{
        j["id"] = id();
        j["parent_id"] = parent_id();
        j["name"] = name();

        std::vector<nlohmann::json> ins,outs;
        nlohmann::json tmp;
        for(auto &gt:this->ins){
            gt->to_json(tmp);
            ins.emplace_back(std::move(tmp));
        }
        for(auto &gt:this->outs){
            gt->to_json(tmp);
            outs.emplace_back(std::move(tmp));
        }
        j["ins"] = std::move(ins);
        j["outs"] = std::move(outs);
    }
};
