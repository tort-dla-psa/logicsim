#pragma once
#include <vector>

namespace bits{

enum class bit_order{
    LSB,
    MSB
};

template<class T>
auto to_bits(const T &val, bit_order order = bit_order::LSB){
    constexpr auto size = sizeof(T);
    std::vector<bool> vec;
    if(order == bit_order::LSB){
        for(size_t i=0; i<size; i++){
            bool bit = (val>>i)&1;
            vec.emplace_back(bit);
        }
    }else{
        for(size_t i=0; i<size; i++){
            bool bit = (val>>size-1-i)&1;
            vec.emplace_back(bit);
        }
    }
    return vec;
}

template<class T>
T from_bits(const std::vector<bool> &vec, bit_order order = bit_order::LSB){
    constexpr auto size = sizeof(T);
    T val = T(0);
    if(order == bit_order::LSB){
        for(size_t i=0; i<size && i<vec.size(); i++){
            val |= vec.at(i) << i;
        }
    }else{
        for(size_t i=0; i<size && i<vec.size(); i++){
            val <<= 1;
            val |= vec.at(i);
        }
        val <<= (size - vec.size());
    }
    return vec;
}

};