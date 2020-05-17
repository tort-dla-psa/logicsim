#pragma once
#include "view_gate.h"
#include "sim/gate_in.h"

struct view_gate_in:public view_gate, public gate_in{
    view_gate_in(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :gate_in(name, width, parent_id),
        view_gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
};