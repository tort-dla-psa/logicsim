#include "sim/utils.h"

template<class T>
T* try_type(const nlohmann::json &j){
    auto tmp = new T();
    if(tmp->from_json(j)){
        return tmp;
    }
    return nullptr;
}

element* utils::from_json_el(const nlohmann::json &j){
    element* el;
    el = try_type<elem_and>(j); if(el) return el;
    el = try_type<elem_nand>(j); if(el) return el;
    el = try_type<elem_or>(j); if(el) return el;
    el = try_type<elem_nor>(j); if(el) return el;
    el = try_type<elem_xor>(j); if(el) return el;
    el = try_type<elem_not>(j); if(el) return el;
    el = try_type<elem_in>(j); if(el) return el;
    el = try_type<elem_out>(j); if(el) return el;
}
element* utils::from_json_gt(const nlohmann::json &j);