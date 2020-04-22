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

};