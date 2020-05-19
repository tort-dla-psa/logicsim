#pragma once
#include "element.h"

class elem_basic:public element{
    using element::insert;
    using element::erase;
    using element::emplace_back;
public:
    elem_basic(const std::string &name)
        :element(name),
        nameable(name)
    {}
};

class elem_and:public elem_basic{
public:
    elem_and(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({in1->get_value(0) && in2->get_value(0)});
        this->processed = true;
    }
};

class elem_nand:public elem_basic{
public:
    elem_nand(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({!(in1->get_value(0) && in2->get_value(0))});
        this->processed = true;
    }
};

class elem_or:public elem_basic{
public:
    elem_or(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({in1->get_value(0) || in2->get_value(0)});
        this->processed = true;
    }
};

class elem_nor:public elem_basic{
public:
    elem_nor(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({!(in1->get_value(0) || in2->get_value(0))});
        this->processed = true;
    }
};

class elem_xor:public elem_basic{
public:
    elem_xor(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({in1->get_value(0) != in2->get_value(0)});
        this->processed = true;
    }
};

class elem_xnor:public elem_basic{
public:
    elem_xnor(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto in2 = std::make_shared<gate_in>(name+"+in_1", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &in2 = ins[1];
        auto &out1 = outs[0];
        out1->pass_value({in1->get_value(0) == in2->get_value(0)});
        this->processed = true;
    }
};

class elem_not:public elem_basic{
public:
    elem_not(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        auto in1 = std::make_shared<gate_in>(name+"+in_0", 1, this->get_id());
        auto out1 = std::make_shared<gate_out>(name+"+out_1", 1, this->get_id());
        element::emplace_back(in1);
        element::emplace_back(out1);
    }
    ~elem_not(){}

    void process()override{
        if(get_processed()){
            return;
        }
        auto &in1 = ins[0];
        auto &out1 = outs[0];
        out1->pass_value({!in1->get_value(0)});
        this->processed = true;
    }
};

template<class Gt, class Gt_outer>
class elem_gate:public elem_basic, public Gt{
    using element::get_outs_begin;
    using element::get_outs_rbegin;
    using element::get_outs_end;
    using element::get_outs_cend;
    using element::get_outs_rend;
    using element::get_ins_begin;
    using element::get_ins_rbegin;
    using element::get_ins_end;
    using element::get_ins_cend;
    using element::get_ins_rend;
    using element::get_ins;
    using element::get_outs;

protected:
    friend class sim;
    friend class elem_file_saver;
    std::shared_ptr<Gt> gt;
    std::shared_ptr<Gt_outer> gt_outer;
public:
    elem_gate(const std::string &name, const std::string &gate_name, const size_t &width=1)
        :elem_basic(name),
        nameable(name),
        Gt(gate_name, width)
    {
        gt = std::make_shared<Gt>(gate_name, width);
        gt_outer = std::make_shared<Gt_outer>(name, width);
    }

    void set_width(const size_t &width)override         { gt->set_width(width); }
    const size_t& get_width()const override             { return gt->get_width(); }
    bool get_value(const size_t &place)const override   { return gt->get_value(place); }
    const std::vector<bool>& get_value()const override { return gt->get_value(); }

    std::shared_ptr<const gate> find_gate(const size_t &id)const override{
        if(gt->get_id() == id){
            auto gt_ptr = std::dynamic_pointer_cast<gate>(gt);
            return std::const_pointer_cast<const gate>(gt_ptr);
        }
        return nullptr;
    }
    std::shared_ptr<gate> find_gate(const size_t &id)override{
        if(gt->get_id() == id){
            return gt;
        }
        return nullptr;
    }
    size_t get_outer_id()const{
        return gt_outer->get_id();
    }
};
class elem_out:public elem_gate<gate_in, gate_out>{
    using element::get_out;
    friend class sim;
public:
    elem_out(const std::string &name, const size_t &width=1)
        :elem_gate(name, name+"_out", width),
        nameable(name)
    {
        elem_gate::gt_outer->parent_id = this->get_id();
        elem_gate::gt->parent_id = this->get_id();
        this->element::ins.emplace_back(elem_gate::gt);
    }
    ~elem_out(){}

    void process()override{
        gt_outer->pass_value(gt->get_value());
    }

    void set_value(const std::vector<bool> &values)override{
        gt->set_value(values);
    }

    size_t get_ins_size()const override { return 1; }
    size_t get_outs_size()const override{ return 0; }

    std::shared_ptr<const gate_in> get_in(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_in should not be called with index != 0");
        return nullptr;
    }

    std::shared_ptr<gate_in> get_in(const size_t &place)override{
        auto c_this = const_cast<const elem_out*>(this);
        auto c_gt = c_this->get_in(place);
        return std::const_pointer_cast<gate_in>(c_gt);
    }
};

class elem_in:public elem_gate<gate_out, gate_in>{
    using element::get_in;
    friend class sim;
public:
    elem_in(const std::string &name, const size_t &width=1)
        :elem_gate(name, name+"_in", width),
        nameable(name)
    {
        elem_gate::gt_outer->parent_id = this->get_id();
        elem_gate::gt->parent_id = this->get_id();
        this->element::outs.emplace_back(elem_gate::gt);
    }
    ~elem_in(){}

    void process()override{
        gt->pass_value(gt_outer->get_value());
    }

    void set_value(const std::vector<bool> &values)override{
        gt_outer->set_value(values);
    }

    size_t get_ins_size()const override { return 0; }
    size_t get_outs_size()const override{ return 1; }

    std::shared_ptr<const gate_out> get_out(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_out should not be called with index != 0");
        return nullptr;
    }

    std::shared_ptr<gate_out> get_out(const size_t &place)override{
        auto c_this = const_cast<const elem_in*>(this);
        auto c_gt = c_this->get_out(place);
        return std::const_pointer_cast<gate_out>(c_gt);
    }
};