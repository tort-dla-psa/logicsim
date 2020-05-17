#pragma once
#include <fstream>
#include <functional>
#include <iterator>
#include <filesystem>
#include <vector>
#include "meta_element.h"
#include "basic_elements.h"
#include "helpers.h"
#include "k_tree.hpp"
#include "sim_ui_glue.h"
#include "nlohmann/json.hpp"

class elem_file_saver{
    using k_tree_sim = k_tree::tree<std::unique_ptr<element>>;
    using k_tree_glue = k_tree::tree<std::shared_ptr<elem_view>>;
    using k_tree_sim_it = k_tree_sim::depth_first_iterator;
    using k_tree_glue_it = k_tree_glue::depth_first_iterator;

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
        if(dynamic_cast<const gate_in*>(gt)){           return types_gate::t_gt_in;
        }else if(dynamic_cast<const gate_out*>(gt)){    return types_gate::t_gt_out;
        }else{
            throw std::runtime_error("unknown type of gate to make gate_type");
        }
        return types_gate::t_gt_in; //unreachable
    }

    static types_gate p_gate_to_type(const gate_view* gt){
        if(dynamic_cast<const gate_view_in*>(gt)){           return types_gate::t_gt_in;
        }else if(dynamic_cast<const gate_view_out*>(gt)){    return types_gate::t_gt_out;
        }else{
            throw std::runtime_error("unknown type of gate to make gate_type");
        }
        return types_gate::t_gt_in; //unreachable
    }

    static std::shared_ptr<gate> p_type_to_gate(types_gate type, const std::string name){
        if(type == types_gate::t_gt_in){        return std::make_shared<gate_in>(name);
        }else if(type == types_gate::t_gt_out){ return std::make_shared<gate_out>(name);
        }else{
            throw std::runtime_error("unknown type of gate to make gate");
        }
        return nullptr; //unreachable
    }

    static std::shared_ptr<gate_view> p_type_to_gate_view(types_gate type, const std::string name){
        std::shared_ptr<gate_view> view;
        if(type == types_gate::t_gt_in){        view = std::make_shared<gate_view_in>();
        }else if(type == types_gate::t_gt_out){ view = std::make_shared<gate_view_out>();
        }else{
            throw std::runtime_error("unknown type of gate to make gate");
        }
        view->name = name;
        return view;
    }

    static types_elem p_elem_to_type(const elem_view* elem){
        if(dynamic_cast<const elem_view_meta*>(elem)){       return types_elem::t_meta;
        }else if(dynamic_cast<const elem_view_and*>(elem)){  return types_elem::t_and;
        }else if(dynamic_cast<const elem_view_or*>(elem)){   return types_elem::t_or;
        }else if(dynamic_cast<const elem_view_not*>(elem)){  return types_elem::t_not;
        }else if(dynamic_cast<const elem_view_in*>(elem)){   return types_elem::t_in;
        }else if(dynamic_cast<const elem_view_out*>(elem)){  return types_elem::t_out;
        }else{
            throw std::runtime_error("unknown type of element view to make element_type");
        }
        return types_elem::t_meta; //unreachable
    }

    static types_elem p_elem_to_type(const element* elem){
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

    static std::shared_ptr<elem_view> p_type_to_elem_view(const types_elem &type, const std::string &name){
        std::shared_ptr<elem_view> view;
        if(type == types_elem::t_meta){         view = std::make_shared<elem_view_meta>();
        }else if(type == types_elem::t_and){    view = std::make_shared<elem_view_and>();
        }else if(type == types_elem::t_or){     view = std::make_shared<elem_view_or>();
        }else if(type == types_elem::t_not){    view = std::make_shared<elem_view_not>();
        }else if(type == types_elem::t_in){     view = std::make_shared<elem_view_in>();
        }else if(type == types_elem::t_out){    view = std::make_shared<elem_view_out>();
        }else{
            throw std::runtime_error("unknown type of element_type to make element");
        }
        view->name = name;
        return view;
    }

    template<class It>
    void retie(It begin, It end){
        for(auto el_it = begin; el_it != end; el_it++){
            auto &el = *el_it;
            for(auto &out:el->outs){
                //replace all "tied" placeholders in an out with real ins of other elements
                for(auto &tied:out->ins){
                    //get placeholder ids
                    auto tied_id = tied->id;
                    auto tied_parent_id = tied->parent_id;
                    //search placeholder parent
                    auto el_it = std::find_if(begin, end,
                        [this, &tied_parent_id](const auto &el){
                            return el->id == tied_parent_id;
                        });
                    if(el_it == end){
                        auto mes = "input id="+std::to_string(tied_id)+
                            " tied to an unknown element id="+std::to_string(tied_parent_id);
                        throw std::runtime_error(mes);
                    }
                    //search real input in placeholder's parent
                    auto &el_parent = *el_it;
                    auto &parent_ins = el_parent->ins;
                    auto in_it = std::find_if(parent_ins.begin(), parent_ins.end(),
                        [this, &tied_id](const auto &gt){
                            return gt->id == tied_id;
                        });
                    if(in_it == parent_ins.end()){
                        //if gate's parent doesn't contain desired gate,
                        //try redo search in it's parent in case
                        //if it's elem_in/elem_out inside elem_meta
                        auto el_parent_id = el_parent->parent_id;
                        auto el_it = std::find_if(begin, end,
                            [&el_parent_id](const auto &el){
                                return el->id == el_parent_id;
                            });
                        auto &el_parent_parent = *el_it;
                        auto &parent_ins = el_parent_parent->ins;
                        in_it = std::find_if(parent_ins.begin(), parent_ins.end(),
                            [this, &tied_id](const auto &gt){
                                return gt->id == tied_id;
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
    auto make_tree(std::vector<T>& elems){
        auto root_it = std::find_if(elems.begin(), elems.end(), 
            [this](const auto &el){
                return el->id == 0;
        });
        auto index = std::distance(root_it, elems.begin());
        auto root = std::move(elems.at(index));
        elems.erase(root_it);

        k_tree::tree<T> tree(std::move(root));
        for(auto &el:elems){
            const auto &parent_id = el->parent_id;
            auto it = std::find_if(tree.begin(), tree.end(),
                [this, &parent_id](const auto &el){
                    return el->id == parent_id;
            });
            if(it != tree.end()){
                tree.append_child(it, std::move(el));
            }
        }
        return tree;
    }

public:

auto sim_to_json(k_tree_sim_it beg, k_tree_sim_it end){
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
        {
            auto elem_in_cast = dynamic_cast<elem_in*>(elem);
            if(elem_in_cast){
                result["gate_outer"] = gate_to_json(elem_in_cast->gt_outer.get());
            }
        } {
            auto elem_out_cast = dynamic_cast<elem_out*>(elem);
            if(elem_out_cast){
                result["gate_outer"] = gate_to_json(elem_out_cast->gt_outer.get());
            }
        }
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

auto sim_from_json(const nlohmann::json &j){
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
            auto gt_ptr = gate_from_json(j_obj);
            auto cast = std::dynamic_pointer_cast<gate_in>(gt_ptr);
            ins.emplace_back(cast);
        }
        for(auto &j_obj:j.at("outs")){
            auto gt_ptr = gate_from_json(j_obj);
            auto cast = std::dynamic_pointer_cast<gate_out>(gt_ptr);
            outs.emplace_back(cast);
        }
        {
            auto elem_in_cast = dynamic_cast<elem_in*>(el.get());
            if(elem_in_cast){
                auto gt_ptr = gate_from_json(j.at("gate_outer"));
                elem_in_cast->gt_outer = std::dynamic_pointer_cast<gate_in>(gt_ptr);
                elem_in_cast->gt = outs.at(0);
                elem_in_cast->gt->parent_id = j.at("id");
                elem_in_cast->gt_outer->parent_id = j.at("id");
            }
        } {
            auto elem_out_cast = dynamic_cast<elem_out*>(el.get());
            if(elem_out_cast){
                auto gt_ptr = gate_from_json(j.at("gate_outer"));
                elem_out_cast->gt_outer = std::dynamic_pointer_cast<gate_out>(gt_ptr);
                elem_out_cast->gt = ins.at(0);
                elem_out_cast->gt->parent_id = j.at("id");
                elem_out_cast->gt_outer->parent_id = j.at("id");
            }
        }
        return el;
    };
    std::vector<std::unique_ptr<element>> elems;
    for(const auto &j_obj:j){
        elems.emplace_back(elem_from_json(j_obj));
    }
    auto tree = make_tree(elems);
    retie(tree.begin(), tree.end());
    return tree;
}

auto glue_to_json(k_tree_glue_it beg, k_tree_glue_it end){
    auto view_to_json = [](const view* v){
        nlohmann::json view_info{
            {"x", v->x},
            {"y", v->y},
            {"w", v->w},
            {"h", v->h},
            {"name", v->name},
            {"id", v->id},
            {"parent_id", v->parent_id},
        };
        return view_info;
    };

    auto gate_to_json = [&view_to_json](const gate_view* gt){
        auto gt_info = view_to_json(gt);
        gt_info["width"]= gt->bit_width;
        gt_info["direction"]= (int)gt->dir;
        gt_info["type"]= p_gate_to_type(gt);

        auto out_cast = dynamic_cast<const gate_view_out*>(gt);
        if(out_cast){
            auto &conns = out_cast->ins;
            std::vector<std::pair<size_t, size_t>> ids;
            ids.reserve(conns.size());
            for(auto &conn:conns){
                ids.emplace_back(conn->id, conn->parent_id);
            }
            gt_info["tied"] = std::move(ids);
        }
        return gt_info;
    };

    auto elem_to_json = [&gate_to_json, &view_to_json](const elem_view* elem){
        auto result = view_to_json(elem);
        result["type"] = p_elem_to_type(elem);
        std::vector<nlohmann::json> ins,outs;
        for(auto &gt:elem->ins){
            ins.emplace_back(gate_to_json(gt.get()));
        }
        for(auto &gt:elem->outs){
            outs.emplace_back(gate_to_json(gt.get()));
        }
        result["ins"] = std::move(ins);
        result["outs"] = std::move(outs);
        {
            auto elem_in_view = dynamic_cast<const elem_view_in*>(elem);
            if(elem_in_view){
                result["gate_outer"] = gate_to_json(elem_in_view->gt_outer.get());
            }
        } {
            auto elem_out_view = dynamic_cast<const elem_view_out*>(elem);
            if(elem_out_view){
                result["gate_outer"] = gate_to_json(elem_out_view->gt_outer.get());
            }
        }
        return result;
    };

    nlohmann::json result;
    for(auto view_it = beg; view_it != end; view_it++){
        auto &view = *view_it;
        auto j = elem_to_json(view.get());
        result += j;
    }
    return result;
}

auto glue_from_json(const nlohmann::json &j){
    auto view_from_json = [](std::shared_ptr<view> v, const nlohmann::json &j){
        v->x = j.at("x");
        v->y = j.at("y");
        v->w = j.at("w");
        v->h = j.at("h");
        v->name = j.at("name");
        v->id = j.at("id");
        v->parent_id = j.at("parent_id");
    };
    auto gate_from_json = [this, &view_from_json](const nlohmann::json& j) {
        auto type = j.at("type");
        auto gt = p_type_to_gate_view(type, j.at("name"));
        view_from_json(gt, j);
        gt->bit_width = j.at("width");
        if(type == types_gate::t_gt_out){
            auto gt_out = std::dynamic_pointer_cast<gate_view_out>(gt);
            std::vector<std::pair<size_t, size_t>> placeholders;
            j.at("tied").get_to(placeholders);
            for(auto &p:placeholders){
                auto in = std::make_shared<gate_view_in>();
                in->parent_id = p.second;
                in->id = p.first;
                gt_out->ins.emplace_back(in);
            }
        }
        return gt;
    };

    auto elem_from_json = [this, &gate_from_json, &view_from_json](const nlohmann::json& j) {
        auto el = p_type_to_elem_view(j.at("type"), j.at("name"));
        view_from_json(el, j);
        auto &ins = el->ins;
        auto &outs = el->outs;
        ins.clear();
        outs.clear();
        for(auto &j_obj:j.at("ins")){
            auto gt = gate_from_json(j_obj);
            ins.emplace_back(std::dynamic_pointer_cast<gate_view_in>(gt));
        }
        for(auto &j_obj:j.at("outs")){
            auto gt = gate_from_json(j_obj);
            outs.emplace_back(std::dynamic_pointer_cast<gate_view_out>(gt));
        }
        {
            auto elem_view_in_cast = std::dynamic_pointer_cast<elem_view_in>(el);
            if(elem_view_in_cast){
                auto gt_ptr = gate_from_json(j.at("gate_outer"));
                elem_view_in_cast->gt_outer = std::dynamic_pointer_cast<gate_view_in>(gt_ptr);
            }
        } {
            auto elem_view_out_cast = std::dynamic_pointer_cast<elem_view_out>(el);
            if(elem_view_out_cast){
                auto gt_ptr = gate_from_json(j.at("gate_outer"));
                elem_view_out_cast->gt_outer = std::dynamic_pointer_cast<gate_view_out>(gt_ptr);
            }
        }
        return el;
    };

    std::vector<std::shared_ptr<elem_view>> elems;
    for(const auto &j_obj:j){
        elems.emplace_back(elem_from_json(j_obj));
    }
    auto tree = make_tree(elems);
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