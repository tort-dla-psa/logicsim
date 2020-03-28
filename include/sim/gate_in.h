#pragma once
#include "gate.h"

class gate_in:public gate{
public:
    gate_in(const std::string &name, size_t width=1)
        :gate(name, width)
    {}
    virtual ~gate_in(){}
};
