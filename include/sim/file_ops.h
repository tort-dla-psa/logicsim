#pragma once
#include <fstream>
#include <functional>
#include <iterator>
#include <filesystem>
#include <vector>
#include "meta_element.h"
#include "basic_elements.h"
#include "helpers.h"
#include "k_tree.h"
#include "nlohmann/json.hpp"

class elem_file_saver{
    using k_tree_ = tree_ns::k_tree<std::unique_ptr<element>>;
    using k_tree_it = k_tree_::depth_first_node_first_iterator;

    enum class types_gate{
        t_gt_in,
        t_gt_out
    };

    enum class types_elem{
        t_meta,
        t_and,
        t_or,
        t_not,
        t_in,
        t_out
    };

    static types_gate p_gate_to_type(const gate* gt){
        if(dynamic_cast<const gate_in*>(gt)){
            return types_gate::t_gt_in;
        }else if(dynamic_cast<const gate_out*>(gt)){
            return types_gate::t_gt_out;
        }else{
            throw std::runtime_error("unknown type of gate to make gate_type");
        }
        return types_gate::t_gt_in; //unreachable
    }

    static std::shared_ptr<gate> p_type_to_gate(types_gate type, const std::string name){
        if(type == types_gate::t_gt_in){
            return std::make_shared<gate_in>(name);
        }else if(type == types_gate::t_gt_out){
            return std::make_shared<gate_out>(name);
        }else{
            throw std::runtime_error("unknown type of gate to make gate");
        }
        return nullptr; //unreachable
    }

    static types_elem p_elem_to_type(const element* elem){
        using namespace sim_helpers;

        if(dynamic_cast<const elem_meta*>(elem)){       return types_elem::t_meta;
        }else if(dynamic_cast<const elem_and*>(elem)){  return types_elem::t_and;
        }else if(dynamic_cast<const elem_or*>(elem)){   return types_elem::t_or;
        }else if(dynamic_cast<const elem_not*>(elem)){  return types_elem::t_not;
        }else if(dynamic_cast<const elem_in*>(elem)){   return types_elem::t_in;
        }else if(dynamic_cast<const elem_out*>(elem)){  return types_elem::t_out;
        }else{
            throw std::runtime_error("unknown type of element to make element_type");
        }
        return types_elem::t_meta; //unreachable
    }

    static std::unique_ptr<element> p_type_to_elem(const types_elem &type, const std::string &name){
        if(type == types_elem::t_meta){         return std::make_unique<elem_meta>(name);
        }else if(type == types_elem::t_and){    return std::make_unique<elem_and>(name);
        }else if(type == types_elem::t_or){     return std::make_unique<elem_or>(name);
        }else if(type == types_elem::t_not){    return std::make_unique<elem_not>(name);
        }else if(type == types_elem::t_in){     return std::make_unique<elem_in>(name);
        }else if(type == types_elem::t_out){    return std::make_unique<elem_out>(name);
        }else{
            throw std::runtime_error("unknown type of element_type to make element");
        }
        return nullptr; //unreachable
    }

    void retie(k_tree_it begin, k_tree_it end){
        for(auto el_it = begin; el_it != end; el_it++){
            auto &el = *el_it;
            for(auto &out:el->get_outs()){
                //replace all "tied" placeholders in an out with real ins of other elements
                for(auto &tied:out->get_tied()){
                    //get placeholder ids
                    auto tied_id = tied->get_id();
                    auto tied_parent_id = tied->get_parent_id();
                    //search placeholder parent
                    auto el_it = std::find_if(begin, end,
                        [&tied_parent_id](const auto &el){
                            return el->get_id() == tied_parent_id;
                        });
                    if(el_it == end){
                        auto mes = "input id="+std::to_string(tied_id)+
                            " tied to an unknown element id="+std::to_string(tied_parent_id);
                        throw std::runtime_error(mes);
                    }
                    //search real input in placeholder's parent
                    auto &el_parent = *el_it;
                    auto in_it = std::find_if(el_parent->get_ins_begin(), el_parent->get_ins_end(),
                        [&tied_id](const auto &gt){
                            return gt->get_id() == tied_id;
                        });
                    if(in_it == el_parent->get_ins_end()){
                        //if gate's parent doesn't contain desired gate,
                        //try redo search in it's parent in case
                        //if it's elem_in/elem_out inside elem_meta
                        auto el_parent_id = el_parent->parent_id;
                        auto el_it = std::find_if(begin, end,
                            [&el_parent_id](const auto &el){
                                return el->get_id() == el_parent_id;
                            });
                        auto &el_parent_parent = *el_it;
                        in_it = std::find_if(el_parent_parent->get_ins_begin(),
                            el_parent_parent->get_ins_end(),
                            [&tied_id](const auto &gt){
                                return gt->get_id() == tied_id;
                            });
                        if(in_it == el_parent_parent->get_ins_end()){
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

    auto make_tree(std::vector<std::unique_ptr<element>>& elems){
        auto root_it = std::find_if(elems.begin(), elems.end(), 
            [](const auto &el){
                return el->get_id() == 0;
        });
        auto index = std::distance(root_it, elems.begin());
        auto root = std::move(elems.at(index));
        elems.erase(root_it);

        k_tree_ tree(std::move(root));
        for(auto &el:elems){
            auto parent_id = el->get_parent_id();
            auto it = std::find_if(tree.begin(), tree.end(),
                [&parent_id](const auto &el){
                    return el->get_id() == parent_id;
            });
            if(it != tree.end()){
                tree.child_append(it, std::move(el));
            }
        }
        return tree;
    }

public:

template<class It>
auto to_json(It beg, It end){
    using namespace sim_helpers;

    auto gate_to_json = [](const gate* gt){
        nlohmann::json gt_info{
            {"id", gt->get_id()},
            {"parent_id", gt->get_parent_id()},
            {"name", gt->get_name()},
            {"width", gt->get_width()},
        };
        auto type = p_gate_to_type(gt);
        gt_info["type"] = type;
        if(type == types_gate::t_gt_out){
            auto gt_out = dynamic_cast<const gate_out*>(gt);
            auto &tied = gt_out->get_tied();
            std::vector<std::pair<size_t, size_t>> ids;
            ids.reserve(tied.size());
            for(auto &in:tied){
                ids.emplace_back(in->get_id(), in->get_parent_id());
            }
            gt_info["tied"] = std::move(ids);
        }
        return gt_info;
    };

    auto elem_to_json = [&gate_to_json](const auto &elem){
        nlohmann::json result{
            {"id", elem->get_id()},
            {"parent_id", elem->get_parent_id()},
            {"name", elem->name},
            {"type", p_elem_to_type(elem)},
            };
        std::vector<nlohmann::json> ins,outs;
        for(auto &gt:elem->ins){
            ins.emplace_back(gate_to_json(gt.get()));
        }
        for(auto &gt:elem->outs){
            outs.emplace_back(gate_to_json(gt.get()));
        }
        result["ins"] = std::move(ins);
        result["outs"] = std::move(outs);
        return result;
    };

    nlohmann::json result;
    while(beg != end){
        const auto &elem = *beg; //u_ptr
        result += elem_to_json(elem.get());
        beg++;
    }
    return result;
}

auto from_json(const nlohmann::json &j){
    auto gate_from_json = [this](const nlohmann::json& j) {
        auto type = j.at("type");
        auto gt = p_type_to_gate(type, j.at("name"));
        gt->id = j.at("id");
        gt->parent_id = j.at("parent_id");
        gt->width = j.at("width");
        if(type == types_gate::t_gt_out){
            auto gt_out = std::dynamic_pointer_cast<gate_out>(gt);
            std::vector<std::pair<size_t, size_t>> placeholders;
            j.at("tied").get_to(placeholders);
            for(auto &p:placeholders){
                auto in = std::make_shared<gate_in>("placeholder", 1, p.second);
                in->id = p.first;
                gt_out->tie_input(in);
            }
        }
        return gt;
    };
    auto elem_from_json = [this, &gate_from_json](const nlohmann::json& j) {
        auto el = p_type_to_elem(j.at("type"), j.at("name"));
        el->id = j.at("id");
        el->parent_id = j.at("parent_id");
        auto &ins = el->get_ins();
        auto &outs = el->get_outs();
        ins.clear();
        outs.clear();
        for(auto &j_obj:j.at("ins")){
            auto gt = gate_from_json(j_obj);
            ins.emplace_back(std::dynamic_pointer_cast<gate_in>(gt));
        }
        for(auto &j_obj:j.at("outs")){
            auto gt = gate_from_json(j_obj);
            outs.emplace_back(std::dynamic_pointer_cast<gate_out>(gt));
        }
        return el;
    };
    std::vector<std::unique_ptr<element>> elems;
    for(const auto &j_obj:j){
        elems.emplace_back(elem_from_json(j_obj));
    }
    k_tree_ tree = make_tree(elems);
    retie(tree.begin(), tree.end());
    return tree;
}

inline void save_json(const nlohmann::json &j, const std::filesystem::path &path) {
    std::ofstream file(path, std::ios::out);
    if(!file.good()){
        throw std::runtime_error("file create error");
    }
    file << j.dump(4);
    file.close();
}

inline auto load_json(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::in);
    if(!file.good()){
        throw std::runtime_error("file open error");
    }
    nlohmann::json j;
    file >> j;
    file.close();
    return j;
}

};