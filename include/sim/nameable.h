#pragma once
#include <string>

class nameable{
protected:
    friend class elem_file_saver;
    friend class sim;
    friend class gate_in;
    friend class gate_out;
    friend class elem_in;
    friend class elem_out;
    std::string m_name;
    size_t m_id, m_parent_id;

    inline static size_t last_id=-1;
public:
    nameable(const std::string &name, const size_t &parent_id=0) {
        m_name = name;
        m_id = ++last_id;
        m_parent_id = parent_id;
    }
    virtual size_t id()const{
        return m_id;
    }
    virtual size_t parent_id()const{
        return m_parent_id;
    }
    const std::string& name()const{
        return m_name;
    }
    std::string name(){
        return m_name;
    }
    void set_name(const std::string &name){
        m_name = name;
    }

    friend bool operator==(const nameable &lhs, const nameable &rhs){
        return lhs.m_id == rhs.m_id &&
            lhs.m_name == rhs.m_name;
    }
    friend bool operator!=(const nameable &lhs, const nameable &rhs){
        return !(lhs == rhs);
    }
};
