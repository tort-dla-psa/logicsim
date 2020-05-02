#pragma once
#include <vector>
#include <memory>
#include "element.h"
#include "basic_elements.h"

class sim;
class elem_meta:public element{
public:
protected:
public:
    elem_meta(const std::string &name, const size_t &parent_id=0)
        :element(name, parent_id),
        nameable(name, parent_id)
    {}
};
