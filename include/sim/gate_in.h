#pragma once
#include "gate.h"
#include "iserializable.h"

class gate_in:public gate{
public:
    gate_in(const std::string &name="gate_in", const size_t &width=1, const size_t &parent_id=0)
        :gate(name, width, parent_id),
        nameable(name, parent_id)
    {}
    ~gate_in(){}

    virtual void to_json(nlohmann::json &j)const override{
        j["token"] = token();
        j["id"] = id();
        j["parent_id"] = parent_id();
        j["name"] = name();
        j["width"] = width();
    }
    virtual bool from_json(const nlohmann::json &j)override{
        if(j.at("token") != token()){
            return false;
        }
        nameable::m_id = j.at("id");
        nameable::m_parent_id = j.at("parent_id");
        nameable::m_name = j.at("name");
        gate::m_width = j.at("width");
        return true;
    }
};
