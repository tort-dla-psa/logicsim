#pragma once
#include <vector>
#include <stdexcept>
#include "nameable.h"

class gate:virtual public nameable{
protected:
    size_t width;
    std::vector<bool> values;
public:
    gate(const std::string &name, const size_t &width)
        :nameable(name)
    {
        set_width(width);
    }
    virtual ~gate(){}

    virtual void set_width(const size_t &width){
        this->width = width;
        values.resize(width);
    }
    virtual const size_t& get_width()const{
        return width;
    }
    virtual bool get_value(const size_t &place)const{
        return values.at(place);
    }
    virtual const std::vector<bool>& get_values()const{
        return values;
    }
    virtual std::vector<bool> get_values(){
        return values;
    }

    virtual void set_values(const std::vector<bool> &values){
        if(values.size() != this->width){
            auto mes = "attempt to assign value of width "+std::to_string(values.size())+
                " to a gate "+get_name()+" with width "+std::to_string(width);
            throw std::runtime_error(mes);
        }
        this->values = values;
    }
};