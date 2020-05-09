#pragma once
#include "view_gate.h"
#include "sim/gate_out.h"

struct view_gate_out:public view_gate, public gate_out{
    view_gate_out(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :gate_out(name, width, parent_id),
        view_gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
};