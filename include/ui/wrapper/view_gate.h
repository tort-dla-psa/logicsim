#pragma once
#include "view_nameable.h"
#include "sim/gate.h"
#include "sim/nameable.h"

struct view_gate:public view_nameable, public gate{
    enum class direction{
        in,
        out
    }dir;
    virtual ~view_gate(){}

protected:
    view_gate(const std::string &name,
        const size_t &width,
        const size_t &parent_id=0)
        :gate(name, width, parent_id),
        view_nameable(name, parent_id),
        nameable(name, parent_id)
    {
        this->w = 10;
        this->h = 10;
    }
};