#pragma once
#include "basic_elements.h"
#include "k_tree.hpp"
#include <nlohmann/json.hpp>
#include "sim/meta_element.h"
#include "sim/basic_elements.h"
#include "k_tree.hpp"

namespace utils{

template<class T>
inline T* try_type(const nlohmann::json &j){
    auto tmp = new T();
    if(tmp->from_json(j)){
        return tmp;
    }
    return nullptr;
}

element* from_json_el(const nlohmann::json &j){
    element* el;
    el = try_type<elem_and>(j); if(el) return el;
    el = try_type<elem_nand>(j); if(el) return el;
    el = try_type<elem_or>(j); if(el) return el;
    el = try_type<elem_nor>(j); if(el) return el;
    el = try_type<elem_xor>(j); if(el) return el;
    el = try_type<elem_xnor>(j); if(el) return el;
    el = try_type<elem_not>(j); if(el) return el;
    el = try_type<elem_in>(j); if(el) return el;
    el = try_type<elem_out>(j); if(el) return el;
    el = try_type<elem_meta>(j); if(el) return el;
    return nullptr;
}
gate* from_json_gt(const nlohmann::json &j){
    gate* el;
    el = try_type<gate_in>(j); if(el) return el;
    el = try_type<gate_out>(j); if(el) return el;
    return nullptr;
}

template<class It>
void retie(It begin, It end){
    for(auto el_it = begin; el_it != end; el_it++){
        auto &el = *el_it;
        for(auto &out:el->outs()){
            //replace all "tied" placeholders in an out with real ins of other elements
            for(auto &tied:out->tied()){
                //get placeholder ids
                auto tied_id = tied->id();
                auto tied_parent_id = tied->parent_id();
                //search placeholder parent
                auto el_it = std::find_if(begin, end,
                    [&tied_parent_id](const auto &el){
                        return el->id() == tied_parent_id;
                    });
                if(el_it == end){
                    auto mes = "input id="+std::to_string(tied_id)+
                        " tied to an unknown element id="+std::to_string(tied_parent_id);
                    throw std::runtime_error(mes);
                }
                //search real input in placeholder's parent
                auto &el_parent = *el_it;
                auto &parent_ins = el_parent->ins();
                auto in_it = std::find_if(parent_ins.begin(), parent_ins.end(),
                    [&tied_id](const auto &gt){
                        return gt->id() == tied_id;
                    });
                if(in_it == parent_ins.end()){
                    //if gate's parent doesn't contain desired gate,
                    //try redo search in it's parent in case
                    //if it's elem_in/elem_out inside elem_meta
                    auto el_parent_id = el_parent->parent_id();
                    auto el_it = std::find_if(begin, end,
                        [&el_parent_id](const auto &el){
                            return el->id() == el_parent_id;
                        });
                    auto &el_parent_parent = *el_it;
                    auto &parent_ins = el_parent_parent->ins();
                    in_it = std::find_if(parent_ins.begin(), parent_ins.end(),
                        [&tied_id](const auto &gt){
                            return gt->id() == tied_id;
                        });
                    if(in_it == parent_ins.end()){
                        auto mes = "element id="+std::to_string(tied_parent_id)+
                            " has no gate_in id="+std::to_string(tied_id);
                        throw std::runtime_error(mes);
                    }
                }
                //retie
                tied = *in_it;
            }
        }
    }
}

template<class T>
k_tree::tree<T> make_tree(std::vector<T>&& elems){
    auto root_it = std::find_if(elems.begin(), elems.end(), 
        [](const auto &el){
            return el->id() == 0;
    });
    auto index = std::distance(root_it, elems.begin());
    auto root = std::move(elems.at(index));
    elems.erase(root_it);

    k_tree::tree<T> tree(std::move(root));
    for(auto &el:elems){
        auto parent_id = el->parent_id();
        auto it = std::find_if(tree.begin(), tree.end(),
            [&parent_id](const auto &el){
                return el->id() == parent_id;
        });
        if(it != tree.end()){
            tree.append_child(it, std::move(el));
        }
    }
    return tree;
}

};
