#pragma once
#include "element.h"

class elem_and:public element{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_and(const std::string &name)
        :element(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_in(in2);
        element::add_out(out1);
    }

    void process()override{
        out1->pass_value({in1->get_value(0) && in2->get_value(0)});
    }
};

class elem_or:public element{
    std::shared_ptr<gate_in> in1, in2;
    std::shared_ptr<gate_out> out1;
public:
    elem_or(const std::string &name)
        :element(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        in2 = std::make_shared<gate_in>(name+"+in_1");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_in(in2);
        element::add_out(out1);
    }

    void process()override{
        out1->pass_value({in1->get_value(0) || in2->get_value(0)});
    }
};

class elem_not:public element{
    std::shared_ptr<gate_in> in1;
    std::shared_ptr<gate_out> out1;
public:
    elem_not(const std::string &name)
        :element(name)
    {
        in1 = std::make_shared<gate_in>(name+"+in_0");
        out1 = std::make_shared<gate_out>(name+"+out_1");
        element::add_in(in1);
        element::add_out(out1);
    }

    void process()override{
        out1->pass_value({!in1->get_value(0)});
    }
};

class elem_out:public element, public gate_out{
public:
    elem_out(const std::string &name, size_t width=1)
        :element(name),
        gate_out(name+"out_1", width)
    {}

    void process()override{
        this->pass_value();
    }
};

class elem_in:public element, public gate_in{
public:
    elem_in(const std::string &name, size_t width=1)
        :element(name),
        gate_in(name+"in_1", width)
    {}

    void process()override{
        //TODO:add logger
    }
};