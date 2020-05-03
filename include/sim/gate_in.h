#pragma once
#include "gate.h"

class gate_in:public gate{
public:
    gate_in(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
    ~gate_in(){}
};

/*
template<class Parent>
class gate_in_active:public gate_in{
    friend class elem_meta;
    friend class elem_file_saver;

    bool m_active;
    Parent* parent;
public:
    gate_in_active(const std::string &name, const size_t &width, Parent *parent)
        :gate_in(name, width, parent->get_id()),
        nameable(name, parent->get_id()),
        parent(parent)
    {}

    gate_in_active(const std::string &name, const size_t &width)
        :gate_in(name, width),
        nameable(name, 0)
    {}

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

    friend bool operator==(const gate_in_active &lhs, const gate_in_active &rhs){
        const nameable &lhs_n(lhs);
        const nameable &rhs_n(rhs);
        const gate_in &lhs_gt(lhs);
        const gate_in &rhs_gt(rhs);
        return lhs_n == rhs_n &&
            lhs_gt == rhs_gt &&
            lhs.parent && rhs.parent &&
            *lhs.parent == *rhs.parent &&
            lhs.m_active == rhs.m_active;
    }
    friend bool operator!=(const gate_in_active &lhs, const gate_in_active &rhs){
        return !(lhs == rhs);
    }
};
*/