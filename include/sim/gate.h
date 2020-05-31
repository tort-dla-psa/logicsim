#pragma once
#include <vector>
#include <stdexcept>
#include "nameable.h"
#include "helpers.h"
#include "logger.h"
#include "iserializable.h"

class gate:virtual public nameable, virtual public ISerializable{
public:
protected:
    friend class elem_file_saver;
    size_t m_width;
    std::vector<bool> m_value;

    gate(const std::string &name, const size_t &width=1, const size_t &parent_id=0)
        :nameable(name, parent_id),
        ISerializable()
    {
        set_width(width);
    }
public:
    virtual ~gate(){}

    virtual void set_width(const size_t &width){
        this->m_width = width;
        m_value.resize(width);
    }
    virtual const size_t& width()const{
        return m_width;
    }
    virtual bool value_at(const size_t &place)const{
        return m_value.at(place);
    }
    virtual const std::vector<bool>& value()const{
        return m_value;
    }

    virtual void set_value(const std::vector<bool> &value){
        if(value.size() != m_width){
            auto mes = "attempt to assign value of width "+std::to_string(value.size())+
                " to a gate "+name()+" with width "+std::to_string(m_width);
            throw std::runtime_error(mes);
        }
        this->m_value = value;
    }

    friend bool operator==(const gate &lhs, const gate &rhs){
        const nameable& lhs_n(lhs);
        const nameable& rhs_n(rhs);
        return lhs_n == rhs_n &&
            lhs.m_width == rhs.m_width;
    }
    friend bool operator!=(const gate &lhs, const gate &rhs){
        return !(lhs == rhs);
    }
};
