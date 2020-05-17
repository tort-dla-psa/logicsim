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