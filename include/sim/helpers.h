#pragma once
#include <string>
#include <vector>

namespace sim_helpers{

inline std::string to_str(const std::vector<bool> &vec){
    std::string result(vec.size(), '0');
    for(size_t i=0; i<vec.size(); i++){
        result.at(i) = vec.at(i)?'1':'0';
    }
    return result;
}

template<class T, class Arg>
inline void push(std::vector<T> &vec, const Arg &arg, const size_t &len = sizeof(Arg)){
    auto cast = reinterpret_cast<const T*>(&arg);
    vec.insert(vec.end(), cast, cast+len);
}

template<class T, class T2>
inline T get_var(T2 &data){
    auto ptr = reinterpret_cast<const T*>(data.base());
    data += sizeof(T);
    return *ptr;
}

};