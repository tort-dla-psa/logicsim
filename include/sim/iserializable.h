#pragma once
#include "nlohmann/json.hpp"
#include <list>

class ISerializable{
    static inline size_t last_token=-1;
protected:
    const size_t m_token;
protected:
    ISerializable()
        :m_token(++last_token)
    {}
    virtual ~ISerializable(){}

    int token()const{ return m_token; }
    virtual bool from_json(const nlohmann::json &j)=0;
    virtual void to_json(nlohmann::json &j)const=0;
};
