#pragma once
#include <vector>
#include <stdexcept>
#include "nameable.h"
#include "helpers.h"
#include "logger.h"

class gate:virtual public nameable{
protected:
    friend class elem_file_saver;
    size_t width;
    std::vector<bool> value;
public:
    gate(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :nameable(name, parent_id)
    {
        set_width(width);
    }
    virtual ~gate(){}

    virtual void set_width(const size_t &width){
        this->width = width;
        value.resize(width);
    }
    virtual const size_t& get_width()const{
        return width;
    }
    virtual bool get_value(const size_t &place)const{
        return value.at(place);
    }
    virtual const std::vector<bool>& get_value()const{
        return value;
    }

    virtual void set_value(const std::vector<bool> &value){
        if(value.size() != this->width){
            auto mes = "attempt to assign value of width "+std::to_string(value.size())+
                " to a gate "+get_name()+" with width "+std::to_string(width);
            throw std::runtime_error(mes);
        }
        this->value = value;
    }

    friend bool operator==(const gate &lhs, const gate &rhs){
        const nameable& lhs_n(lhs);
        const nameable& rhs_n(rhs);
        return lhs_n == rhs_n &&
            lhs.width == rhs.width;
    }
    friend bool operator!=(const gate &lhs, const gate &rhs){
        return !(lhs == rhs);
    }
};