#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "gate_in.h"
#include "gate.h"
#include "helpers.h"
#include "iserializable.h"

class gate_out:public gate{
public:
    using ins_vec = std::vector<std::shared_ptr<gate_in>>;
private:
    friend class elem_file_saver;
    ins_vec m_ins;
public:
    gate_out(const std::string &name="gate_out", const size_t &width=1, const size_t &parent_id=0)
        :gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
    ~gate_out(){}

    void pass_value()const{
        auto &val = this->value();
        for(auto &in:m_ins){
            in->set_value(val);
        }
    }
    void pass_value(const std::vector<bool> &val){
        set_value(val);
        pass_value();
    }
    void tie_input(const std::shared_ptr<gate_in> &in){
        if(in->width() != this->width()){
            auto mes = "attempt to tie input "+in->name()+
                " of width "+std::to_string(in->width())+
                "to an output "+name()+" of width "+std::to_string(width());
            throw std::runtime_error(mes);
        }
        this->m_ins.emplace_back(in);
    }
    void untie_input(const std::shared_ptr<gate_in> &in){
        auto it = std::find(m_ins.begin(), m_ins.end(), in);
        if(it == m_ins.end()){
            auto mes = "attempt to untie input "+in->name()+
                "from an output "+name()+", but they are not tied";
            throw std::runtime_error(mes);
        }
        m_ins.erase(it);
    }
    bool tied(const std::shared_ptr<gate_in> &in){
        auto it = std::find(m_ins.begin(), m_ins.end(), in);
        return !(it == m_ins.end());
    }

    auto tied_begin()       { return m_ins.begin(); }
    auto tied_begin()const  { return m_ins.begin(); }
    auto tied_cbegin()const { return m_ins.begin(); }
    auto tied_rbegin()      { return m_ins.rbegin(); }
    auto tied_rbegin()const { return m_ins.rbegin(); }
    auto tied_crbegin()const{ return m_ins.rbegin(); }
    auto tied_end()         { return m_ins.end(); }
    auto tied_end()const    { return m_ins.end(); }
    auto tied_cend()const   { return m_ins.end(); }
    auto tied_rend()        { return m_ins.rend(); }
    auto tied_rend()const   { return m_ins.rend(); }
    auto tied_crend()const  { return m_ins.rend(); }
    auto& tied()const       { return m_ins; }
    auto& tied()            { return m_ins; }

    friend bool operator==(const gate_out &lhs, const gate_out &rhs){
        const nameable &lhs_n(lhs);
        const nameable &rhs_n(rhs);
        const gate &lhs_g(lhs);
        const gate &rhs_g(rhs);
        bool ins_eq = std::equal(lhs.m_ins.begin(), lhs.m_ins.end(), rhs.m_ins.begin(),
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
    virtual void to_json(nlohmann::json &j)const override{
        j["token"] = token();
        j["id"] = id();
        j["parent_id"] = parent_id();
        j["name"] = name();
        j["width"] = width();
        std::vector<std::pair<size_t, size_t>> ids;
        ids.reserve(m_ins.size());
        for(auto &in:m_ins){
            ids.emplace_back(in->id(), in->parent_id());
        }
        j["tied"] = std::move(ids);
    }
    virtual bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        nameable::m_id = j.at("id");
        nameable::m_parent_id = j.at("parent_id");
        nameable::m_name = j.at("name");
        gate::m_width = j.at("width");
        std::vector<std::pair<size_t, size_t>> placeholders;
        j.at("tied").get_to(placeholders);
        for(auto &p:placeholders){
            auto in = std::make_shared<gate_in>("placeholder", 1, p.second);
            in->nameable::m_id = p.first;
            tie_input(in);
        }
        return false;
    }
};
