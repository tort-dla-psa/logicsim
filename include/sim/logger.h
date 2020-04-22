#pragma once
#include <optional>
#include <iostream>
#include <sstream>
#include <string>

class logger{
    logger(){}
public:
    void log(const std::string &name, const std::string &str){
        std::cout<<name<<":\t"<<str<<"\n";
    }

    ~logger(){
        std::cout.flush();
    }

    static logger& get_instance(){
        static logger lg;
        return lg;
    }
};