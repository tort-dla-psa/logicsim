#pragma once
#include <string>

class nameable{
    std::string name;
    size_t id;

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
    nameable(const std::string &name)
    {
        this->set_name(name);
        this->id = id_assigner::get_instance().get_id();
    }
    virtual ~nameable(){};
    virtual size_t get_id()const{
        return id;
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
};