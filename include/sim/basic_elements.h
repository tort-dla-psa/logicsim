#pragma once
#include "element.h"
#include "nlohmann/json.hpp"

class elem_basic:public element{
    using element::insert;
    using element::erase;
    using element::emplace_back;
public:
    enum class type_elem{
        t_meta,
        t_and,
        t_nand,
        t_or,
        t_nor,
        t_xor,
        t_xnor,
        t_not,
        t_in,
        t_out
    };
protected:
    enum type_elem m_type;
    elem_basic(const std::string &name, enum type_elem type)
        :element(name),
        nameable(name),
        m_type(type)
    {}
public:
    type_elem type()const{ return this->m_type; }
};

class elem_and:public elem_basic{
public:
    elem_and(const std::string &name="and")
        :elem_basic(name, type_elem::t_and),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({in1->value_at(0) && in2->value_at(0)});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_nand:public elem_basic{
public:
    elem_nand(const std::string &name="nand")
        :elem_basic(name, type_elem::t_nand),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({!(in1->value_at(0) && in2->value_at(0))});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_or:public elem_basic{
public:
    elem_or(const std::string &name="or")
        :elem_basic(name, type_elem::t_or),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({in1->value_at(0) || in2->value_at(0)});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_nor:public elem_basic{
public:
    elem_nor(const std::string &name="nor")
        :elem_basic(name, type_elem::t_nor),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({!(in1->value_at(0) || in2->value_at(0))});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_xor:public elem_basic{
public:
    elem_xor(const std::string &name="xor")
        :elem_basic(name, type_elem::t_xor),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({in1->value_at(0) != in2->value_at(0)});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_xnor:public elem_basic{
public:
    elem_xnor(const std::string &name="xnor")
        :elem_basic(name, type_elem::t_xnor),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &in2 = m_ins[1];
        auto &out1 = m_outs[0];
        out1->pass_value({in1->value_at(0) == in2->value_at(0)});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

class elem_not:public elem_basic{
public:
    elem_not(const std::string &name="not")
        :elem_basic(name, type_elem::t_nor),
        ISerializable(),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->id());
        element::emplace_back(in1);
        element::emplace_back(out1);
    }
    ~elem_not(){}

    void process()override{
        if(processed()){
            return;
        }
        auto &in1 = m_ins[0];
        auto &out1 = m_outs[0];
        out1->pass_value({!in1->value_at(0)});
        this->m_processed = true;
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        element::to_json(j);
        j["token"] = token();
    }
};

template<class Gt, class Gt_outer>
class elem_gate:public elem_basic, public Gt{
    using element::outs_begin;
    using element::outs_rbegin;
    using element::outs_end;
    using element::outs_cend;
    using element::outs_rend;
    using element::ins_begin;
    using element::ins_rbegin;
    using element::ins_end;
    using element::ins_cend;
    using element::ins_rend;
    using element::ins;
    using element::outs;
protected:
    friend class sim;
    friend class elem_file_saver;
    std::shared_ptr<Gt> gt;
    std::shared_ptr<Gt_outer> gt_outer;
public:
    elem_gate(const std::string &name, const std::string &gate_name,
        type_elem type, const size_t &width=1)
        :elem_basic(name, type),
        ISerializable(),
        nameable(name),
        Gt(gate_name, width)
    {
        gt = std::make_shared<Gt>(gate_name, width);
        gt_outer = std::make_shared<Gt_outer>(name, width);
    }

    void set_width(const size_t &width)override         { gt->set_width(width); }
    const size_t& width()const override             { return gt->width(); }
    bool value_at(const size_t &place)const override   { return gt->value_at(place); }
    const std::vector<bool>& value()const override { return gt->value(); }

    std::shared_ptr<const gate> find_gate(const size_t &id)const override{
        if(gt->id() == id){
            auto gt_ptr = std::dynamic_pointer_cast<gate>(gt);
            return std::const_pointer_cast<const gate>(gt_ptr);
        }
        return nullptr;
    }
    std::shared_ptr<gate> find_gate(const size_t &id)override{
        if(gt->id() == id){
            return gt;
        }
        return nullptr;
    }
    size_t outer_id()const{
        return gt_outer->id();
    }
    virtual bool from_json(const nlohmann::json &j)override=0;
    virtual void to_json(nlohmann::json &j)const override=0;
};

class elem_out:public elem_gate<gate_in, gate_out>{
    using element::out;
    friend class sim;
public:
    elem_out(const std::string &name="out", const size_t &width=1)
        :elem_gate(name, name+"_out", type_elem::t_out, width),
        nameable(name)
    {
        elem_gate::gt_outer->m_parent_id = this->id();
        elem_gate::gt->m_parent_id = this->id();
        this->element::m_ins.emplace_back(elem_gate::gt);
    }
    ~elem_out(){}

    void process()override{
        gt_outer->pass_value(gt->value());
    }

    void set_value(const std::vector<bool> &values)override{
        gt->set_value(values);
    }

    size_t ins_size()const override { return 1; }
    size_t outs_size()const override{ return 0; }

    std::shared_ptr<const gate_in> in(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_in should not be called with index != 0");
        return nullptr;
    }

    std::shared_ptr<gate_in> in(const size_t &place)override{
        auto c_this = const_cast<const elem_out*>(this);
        auto c_gt = c_this->in(place);
        return std::const_pointer_cast<gate_in>(c_gt);
    }
    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        gt_outer->from_json(j.at("gate_outer"));
        gt = m_ins.at(0);
        gt->m_parent_id = j.at("id");
        gt_outer->m_parent_id = j.at("id");
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        j["token"] = token();
        element::to_json(j);
        nlohmann::json tmp;
        gt_outer->to_json(tmp);
        j["gate_outer"] = std::move(tmp);
    }
};

class elem_in:public elem_gate<gate_out, gate_in>{
    using element::in;
    friend class sim;
public:
    elem_in(const std::string &name="in", const size_t &width=1)
        :elem_gate(name, name+"_in", type_elem::t_in, width),
        nameable(name)
    {
        elem_gate::gt_outer->m_parent_id = this->id();
        elem_gate::gt->m_parent_id = this->id();
        this->element::m_outs.emplace_back(elem_gate::gt);
    }
    ~elem_in(){}

    void process()override{
        gt->pass_value(gt_outer->value());
    }

    void set_value(const std::vector<bool> &values)override{
        gt_outer->set_value(values);
    }

    size_t ins_size()const override { return 0; }
    size_t outs_size()const override{ return 1; }

    std::shared_ptr<const gate_out> out(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_out should not be called with index != 0");
        return nullptr;
    }

    std::shared_ptr<gate_out> out(const size_t &place)override{
        auto c_this = const_cast<const elem_in*>(this);
        auto c_gt = c_this->out(place);
        return std::const_pointer_cast<gate_out>(c_gt);
    }

    bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        element::from_json(j);
        gt_outer->from_json(j.at("gate_outer"));
        gt = m_outs.at(0);
        gt->m_parent_id = j.at("id");
        gt_outer->m_parent_id = j.at("id");
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        j["token"] = token();
        element::to_json(j);
        nlohmann::json tmp;
        gt_outer->to_json(tmp);
        j["gate_outer"] = std::move(tmp);
    }
};
