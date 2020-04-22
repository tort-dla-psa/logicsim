#pragma once
#include "element.h"

class elem_basic:public element{
    using element::insert;
    using element::erase;
    using element::emplace_back;
public:
    elem_basic(const std::string &name)
        :element(name)
    {}
};

class elem_and final :public elem_basic{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_and(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }
    virtual ~elem_and(){}

    void process()override{
        if(get_processed()){
            return;
        }
        out1->pass_value({in1->get_value(0) && in2->get_value(0)});
        this->processed = true;
    }
};

class elem_or final :public elem_basic{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_or(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::emplace_back(in1);
        element::emplace_back(in2);
        element::emplace_back(out1);
    }
    virtual ~elem_or(){}

    void process()override{
        if(get_processed()){
            return;
        }
        out1->pass_value({in1->get_value(0) || in2->get_value(0)});
        this->processed = true;
    }
};

class elem_not final :public elem_basic{
    std::shared_ptr<gate_in> in1;
    std::shared_ptr<gate_out> out1;
public:
    elem_not(const std::string &name)
        :elem_basic(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::emplace_back(in1);
        element::emplace_back(out1);
    }
    virtual ~elem_not(){}

    void process()override{
        if(get_processed()){
            return;
        }
        out1->pass_value({!in1->get_value(0)});
        this->processed = true;
    }
};

template<class Gt>
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
    using element::get_gates_begin;
    using element::get_gates_rbegin;
    using element::get_gates_end;
    using element::get_gates_cend;
    using element::get_gates_rend;
    using element::get_gates;
    using element::get_ins;
    using element::get_outs;

protected:
    std::shared_ptr<Gt> gt;
public:
    elem_gate(const std::string &name, const std::string &gate_name, const size_t &width=1)
        :elem_basic(name),
        Gt(gate_name, width)
    {
        gt = std::make_shared<Gt>(gate_name, width);
    }

    void set_width(const size_t &width)override         { gt->set_width(width); }
    const size_t& get_width()const override             { return gt->get_width(); }
    bool get_value(const size_t &place)const override   { return gt->get_value(place); }
    const std::vector<bool>& get_values()const override { return gt->get_values(); }
    std::vector<bool> get_values()override              { return gt->get_values(); }
    void set_values(const std::vector<bool> &values)override{ gt->set_values(values); }

    std::shared_ptr<const gate> find_gate(const size_t &id)const override{
        if(gt->get_id() == id){
            return std::const_pointer_cast<const gate>(std::dynamic_pointer_cast<gate>(gt));
        }
        return nullptr;
    }
    std::shared_ptr<gate> find_gate(const size_t &id)override{
        if(gt->get_id() == id){
            return gt;
        }
        return nullptr;
    }
};
class elem_out final :public elem_gate<gate_out>, std::enable_shared_from_this<gate_out>{
public:
    elem_out(const std::string &name, size_t width=1)
        :elem_gate<gate_out>(name, name+"out_1", width),
        nameable(name)
    { }
    virtual ~elem_out(){}

    void process()override{
        gt->pass_value();
    }

    size_t get_outs_size()const override{
        return 1;
    }

    size_t get_out_id()const{
        return gate_out::get_id();
    }

    std::shared_ptr<const gate_out> get_out(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_out should not be called");
        return nullptr;
    }

    std::shared_ptr<gate_out> get_out(const size_t &place)override{
        auto c_this = const_cast<const elem_out*>(this);
        auto c_gt = c_this->get_out(place);
        return std::const_pointer_cast<gate_out>(c_gt);
    }
};

class elem_in final :public elem_gate<gate_in>, std::enable_shared_from_this<gate_in>{
public:
    elem_in(const std::string &name, size_t width=1)
        :elem_gate<gate_in>(name, name+"in_1", width),
        nameable(name)
    {}
    virtual ~elem_in(){}

    void process()override{
        //TODO:add logger
    }

    size_t get_ins_size()const override{
        return 1;
    }

    size_t get_in_id()const{
        return gt->get_id();
    }

    std::shared_ptr<const gate_in> get_in(const size_t &place)const override{
        if(place == 0){
            return gt;
        }
        throw std::runtime_error("elem::get_in should not be called");
        return nullptr;
    }

    std::shared_ptr<gate_in> get_in(const size_t &place)override{
        auto c_this = const_cast<const elem_in*>(this);
        auto c_gt = c_this->get_in(place);
        return std::const_pointer_cast<gate_in>(c_gt);
    }
};