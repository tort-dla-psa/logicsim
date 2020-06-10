#pragma once
#include "nlohmann/json.hpp"
#include <list>

class ISerializable{
    static inline size_t last_token=-1;
protected:
    size_t m_token;

    ISerializable()
        :m_token(++last_token)
    {}
public:
    ISerializable(const ISerializable& rhs)
        :m_token(rhs.m_token)
    {}
    auto& operator=(const ISerializable &rhs){
        this->m_token = rhs.m_token;
        return *this;
    }
    virtual ~ISerializable(){}

    int token()const{ return m_token; }
    virtual bool from_json(const nlohmann::json &j)=0;
    virtual void to_json(nlohmann::json &j)const=0;
};
