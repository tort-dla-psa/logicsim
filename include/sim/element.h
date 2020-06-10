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
    std::vector<std::shared_ptr<gate_in>> m_ins;
    std::vector<std::shared_ptr<gate_out>> m_outs;

    bool m_processed;

    element(const std::string &name)
        :nameable(name)
    {
        reset_processed();
    }
public:
    virtual ~element(){}
    virtual void process(){}

    auto outs_begin()       { return m_outs.begin(); }
    auto outs_begin()const  { return m_outs.cbegin(); }
    auto outs_rbegin()      { return m_outs.rbegin(); }
    auto outs_rbegin()const { return m_outs.crbegin(); }
    auto outs_end()         { return m_outs.end(); }
    auto outs_end()const    { return m_outs.end(); }
    auto outs_cend()const   { return m_outs.cend(); }
    auto outs_rend()        { return m_outs.rend(); }
    auto outs_rend()const   { return m_outs.crend(); }
    auto ins_begin()        { return m_ins.begin(); }
    auto ins_begin()const   { return m_ins.cbegin(); }
    auto ins_rbegin()       { return m_ins.rbegin(); }
    auto ins_rbegin()const  { return m_ins.crbegin(); }
    auto ins_end()          { return m_ins.end(); }
    auto ins_end()const     { return m_ins.end(); }
    auto ins_cend()const    { return m_ins.cend(); }
    auto ins_rend()         { return m_ins.rend(); }
    auto ins_rend()const    { return m_ins.crend(); }
    virtual ins_vec& ins()                  { return m_ins; }
    virtual const ins_vec& ins()const       { return m_ins; }
    virtual outs_vec& outs()                { return m_outs; }
    virtual const outs_vec& outs()const     { return m_outs; }
    virtual size_t ins_size()const          { return m_ins.size(); }
    virtual size_t outs_size()const         { return m_outs.size(); }

    virtual std::shared_ptr<const gate> find_gate(const size_t &id)const{
        auto predicate = [&id](auto gate){
            return gate->id() == id;
        };
        auto c_in_it = std::find_if(m_ins.cbegin(), m_ins.cend(), predicate);
        if(c_in_it != m_ins.cend()){
            return *c_in_it;
        }
        auto c_out_it = std::find_if(m_outs.cbegin(), m_outs.cend(), predicate);
        if(c_out_it != m_outs.cend()){
            return *c_out_it;
        }
        return nullptr;
    }
    virtual std::shared_ptr<gate> find_gate(const size_t &id){
        auto c_this = const_cast<const element*>(this);
        auto c_gt = c_this->find_gate(id);
        return std::const_pointer_cast<gate>(c_gt);
    }

    virtual bool processed()const{
        return m_processed;
    }
    virtual void reset_processed(){
        m_processed = false;
    }

    virtual std::shared_ptr<const gate_in> in(const size_t &place)const{
        return m_ins.at(place);
    }
    virtual std::shared_ptr<const gate_out> out(const size_t &place)const{
        return m_outs.at(place);
    }
    virtual std::shared_ptr<gate_in> in(const size_t &place){
        return m_ins.at(place);
    }
    virtual std::shared_ptr<gate_out> out(const size_t &place){
        return m_outs.at(place);
    }
    void emplace_back(const std::shared_ptr<gate_in> in){
        m_ins.emplace_back(in);
    }
    void emplace_back(const std::shared_ptr<gate_out> out){
        m_outs.emplace_back(out);
    }
    void insert(const ins_vec::const_iterator it, const std::shared_ptr<gate_in> &in){
        m_ins.emplace(it, in);
    }
    void insert(const outs_vec::const_iterator it, const std::shared_ptr<gate_out> &out){
        m_outs.emplace(it, out);
    }
    void insert(const std::shared_ptr<gate_in> in, size_t place){
        if(place > m_ins.size()){
            auto mes = "attempt to add input in place "+std::to_string(place)+
                "to element "+name()+", which has "+std::to_string(ins_size())+
                "inputs";
            throw std::runtime_error(mes);
        }
        insert(m_ins.begin()+place, in);
    }
    void insert(const std::shared_ptr<gate_out> out, size_t place){
        if(place > m_outs.size()){
            auto mes = "attempt to add output in place "+std::to_string(place)+
                "to element "+name()+", which has "+std::to_string(outs_size())+
                "outputs";
            throw std::runtime_error(mes);
        }
        insert(m_outs.begin()+place, out);
    }
    void erase(ins_vec::const_iterator it){
        m_ins.erase(it);
    }
    void erase(outs_vec::const_iterator it){
        m_outs.erase(it);
    }

    friend bool operator==(const element &lhs, const element &rhs){
        const nameable &lhs_n(lhs);
        const nameable &rhs_n(rhs);
        bool ins_eq = std::equal(lhs.m_ins.begin(), lhs.m_ins.end(), rhs.m_ins.begin(),
            [](auto ptr1, auto ptr2){
                return (ptr1 && ptr2) && (*ptr1 == *ptr2);
            });
        bool outs_eq = std::equal(lhs.m_outs.begin(), lhs.m_outs.end(), rhs.m_outs.begin(),
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
        m_ins.clear();
        m_outs.clear();
        bool status = true;
        for(auto &j_obj:j.at("ins")){
            auto gt_tmp = std::make_shared<gate_in>("placeholder");
            status &= gt_tmp->from_json(j_obj);
            m_ins.emplace_back(gt_tmp);
        }
        for(auto &j_obj:j.at("outs")){
            auto gt_tmp = std::make_shared<gate_out>("placeholder");
            status &= gt_tmp->from_json(j_obj);
            m_outs.emplace_back(gt_tmp);
        }
        return status;
    }

    virtual void to_json(nlohmann::json &j)const override{
        j["id"] = id();
        j["parent_id"] = parent_id();
        j["name"] = name();

        std::vector<nlohmann::json> ins,outs;
        nlohmann::json tmp;
        for(auto &gt:this->m_ins){
            gt->to_json(tmp);
            ins.emplace_back(std::move(tmp));
        }
        for(auto &gt:this->m_outs){
            gt->to_json(tmp);
            outs.emplace_back(std::move(tmp));
        }
        j["ins"] = std::move(ins);
        j["outs"] = std::move(outs);
    }
};
