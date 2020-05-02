#pragma once
#include <string>

class nameable{
    friend class elem_file_saver;
    friend class elem_in;
    friend class elem_out;
    std::string name;
    size_t id, parent_id;

    class id_assigner{
    private:
        size_t last_id=-1; //to start from zero
        id_assigner(){};
    public:
        static id_assigner& get_instance(){
            static id_assigner inst;
            return inst;
        }
        size_t get_id(){
            last_id++;
            return last_id;
        }
    };
public:
    nameable(const std::string &name, const size_t &parent_id) {
        this->set_name(name);
        this->id = id_assigner::get_instance().get_id();
        this->parent_id = parent_id;
    }
    virtual size_t get_id()const{
        return id;
    }
    virtual size_t get_parent_id()const{
        return parent_id;
    }
    const std::string& get_name()const{
        return name;
    }
    std::string get_name(){
        return name;
    }
    void set_name(const std::string &name){
        this->name = name;
    }

    friend bool operator==(const nameable &lhs, const nameable &rhs){
        return lhs.id == rhs.id &&
            lhs.name == rhs.name;
    }
    friend bool operator!=(const nameable &lhs, const nameable &rhs){
        return !(lhs == rhs);
    }
};