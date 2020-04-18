#pragma once
#include "element.h"

class elem_and:public element{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_and(const std::string &name)
        :element(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_in(in2);
        element::add_out(out1);
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

class elem_or:public element{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_or(const std::string &name)
        :element(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_in(in2);
        element::add_out(out1);
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

class elem_not:public element{
    std::shared_ptr<gate_in> in1;
    std::shared_ptr<gate_out> out1;
public:
    elem_not(const std::string &name)
        :element(name),
        nameable(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_out(out1);
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

class elem_out:public element, public gate_out{
public:
    elem_out(const std::string &name, size_t width=1)
        :element(name),
        gate_out(name+"out_1", width),
        nameable(name)
    {}
    virtual ~elem_out(){}

    void process()override{
        this->pass_value();
    }

    size_t get_outs()const override{
        return 1;
    }

    size_t get_out_id()const{
        return gate_out::get_id();
    }

    std::shared_ptr<gate_out> get_out(size_t place)const override{
        throw std::runtime_error("elem::get_out should not be called");
        return nullptr;
    }
};

class elem_in:public element, public gate_in{
public:
    elem_in(const std::string &name, size_t width=1)
        :element(name),
        gate_in(name+"in_1", width),
        nameable(name)
    {}
    virtual ~elem_in(){}

    void process()override{
        //TODO:add logger
    }

    size_t get_ins()const override{
        return 1;
    }

    size_t get_in_id()const{
        return gate_in::get_id();
    }

    std::shared_ptr<gate_in> get_in(size_t place)const override{
        throw std::runtime_error("elem::get_in should not be called");
        return nullptr;
    }
};