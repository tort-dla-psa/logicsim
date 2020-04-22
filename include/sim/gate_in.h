#pragma once
#include "gate.h"

class gate_in:public gate{
public:
    gate_in(const std::string &name, size_t width=1)
        :gate(name, width),
        nameable(name)
    {}
    ~gate_in(){}
};

template<class Parent>
class gate_in_active:public gate_in{
    friend class elem_meta;
    bool m_active;
    Parent* parent;
public:
    gate_in_active(const std::string &name, size_t width=1,
        Parent *parent=nullptr)
        :gate_in(name, width),
        nameable(name),
        parent(parent)
    {}
    ~gate_in_active(){}

    void set_active(bool active){
        this->m_active = active;
    }

    bool get_active()const{
        return m_active;
    }

    void set_values(const std::vector<bool> &value)override{
        gate::set_values(value);
        if(m_active && parent){
            parent->process();
        }
    }
};