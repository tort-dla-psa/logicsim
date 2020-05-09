#pragma once
#include "sim/nameable.h"

struct view_nameable:virtual public nameable{
    long x, y, w, h;
    enum class state{
        normal,
        creating,
        selected,
        copied,
        cut
    }st = state::normal;

    view_nameable(const std::string &name="generic_name", const size_t &parent_id=-1)
        :nameable(name, parent_id)
    {}
    virtual ~view_nameable(){}
};