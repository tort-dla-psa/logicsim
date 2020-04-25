#pragma once
#include <fstream>
#include <functional>
#include <iterator>
#include <filesystem>
#include <vector>
#include "meta_element.h"
#include "basic_elements.h"
#include "helpers.h"

class elem_file_saver{
    enum class types{
        t_meta = 0,
        t_and,
        t_or,
        t_not,
        t_in,
        t_out
    };

    static types p_elem_to_type(const element* elem){
        using namespace sim_helpers;
        if(dynamic_cast<const elem_meta*>(elem)){
            return types::t_meta;
        }else if(dynamic_cast<const elem_and*>(elem)){
            return types::t_and;
        }else if(dynamic_cast<const elem_or*>(elem)){
            return types::t_or;
        }else if(dynamic_cast<const elem_not*>(elem)){
            return types::t_not;
        }else if(dynamic_cast<const elem_in*>(elem)){
            return types::t_in;
        }else if(dynamic_cast<const elem_out*>(elem)){
            return types::t_out;
        }else{
            throw std::runtime_error("unknown type of element to make element_type");
        }
        return types::t_meta; //unreachable
    }

    static std::unique_ptr<element> p_type_to_elem(const types &type, const std::string &name){
        if(type == types::t_meta){
            return std::make_unique<elem_meta>(name);
        }else if(type == types::t_and){
            return std::make_unique<elem_and>(name);
        }else if(type == types::t_or){
            return std::make_unique<elem_or>(name);
        }else if(type == types::t_not){
            return std::make_unique<elem_not>(name);
        }else if(type == types::t_in){
            return std::make_unique<elem_in>(name);
        }else if(type == types::t_out){
            return std::make_unique<elem_out>(name);
        }else{
            throw std::runtime_error("unknown type of element_type to make element");
        }
        return nullptr; //unreachable
    }

static void retie(std::vector<std::unique_ptr<element>>& elems){
    auto get_gate_by_id = [](auto elems_beg, auto elems_end, auto id) { //YAAAY, recursive lambda!!
        auto get_gate_id_impl = [](auto elems_beg, auto elems_end, auto id, auto &func)
            ->std::shared_ptr<gate>
        {
            for(auto it = elems_beg; it != elems_end; it++){
                auto &el = *it;
                auto meta_cast = dynamic_cast<elem_meta*>(el.get());
                if(meta_cast){
                    return func(meta_cast->get_begin(), meta_cast->get_end(), id, func);
                }
                auto gt_beg = el->get_gates_begin();
                auto gt_end = el->get_gates_end();
                for(auto it = gt_beg; it != gt_end; it++){
                    auto &gt = *it;
                    if(gt->get_id() == id){
                        return gt;
                    }
                }
            }
            return nullptr; //should not reach it
        };
        return get_gate_id_impl(elems_beg, elems_end, id, get_gate_id_impl);
    };

    auto retie_elem = [&elems, &get_gate_by_id](auto &el){ //el is ptr
        auto out_beg = el->get_outs_begin();
        auto out_end = el->get_outs_end();
        for(auto it = out_beg; it != out_end; it++){
            auto tied_beg = (*it)->get_tied_begin();
            auto tied_end = (*it)->get_tied_end();
            for(auto it_tied = tied_beg; it_tied != tied_end; it++){
                auto id = (*it_tied)->get_id();
                auto gate_to_tie = get_gate_by_id(elems.begin(), elems.end(), id);
                auto gate_to_tie_cast = std::dynamic_pointer_cast<gate_in>(gate_to_tie);
                if(!gate_to_tie_cast){
                    auto mes = "gate with id "+std::to_string(id)+" is not a gate_in";
                    throw std::runtime_error(mes);
                }
                *it_tied = gate_to_tie_cast; //replace placeholder with real gate_in
            }
        }
    };

    for(auto &el:elems){
        auto meta_cast = dynamic_cast<elem_meta*>(el.get());
        if(meta_cast){
            auto in_beg = meta_cast->get_begin();
            auto in_end = meta_cast->get_end();
            for(auto it = in_beg; it != in_end; it++){
                retie_elem(*it);
            }
            retie_elem(el);
            continue;
        }
        retie_elem(el);
    }
}

static void from_bin_load(std::vector<uint8_t>::const_iterator &it, std::vector<std::unique_ptr<element>>& elems){
    using namespace sim_helpers;

    std::unique_ptr<element> elem;
    {
        auto id = get_var<decltype(nameable::id)>(it);
        auto name_len = get_var<size_t>(it);
        auto name_chars = reinterpret_cast<const char*>(it.base());
        auto name = std::string(name_chars, name_chars+name_len);
        it += name_len;
        auto type_ = get_var<elem_file_saver::types>(it);
        elem = std::move(p_type_to_elem(type_, name));
        elem->id = id;
    }

    auto get_gate_params = [&it](auto &id, auto &name, auto &width){
        id = get_var<decltype(nameable::id)>(it);
        auto name_len = get_var<size_t>(it);
        auto name_chars = reinterpret_cast<const char*>(it.base());
        name = std::string(name_chars, name_chars+name_len);
        it += name_len;
        width = get_var<decltype(gate::width)>(it);
    };
    decltype(nameable::id) id_tmp;
    std::string name_tmp;
    decltype(gate::width) width_tmp;
    {
        auto ins_size = get_var<std::ptrdiff_t>(it);
        std::vector<std::shared_ptr<gate_in>> ins;
        ins.reserve(ins_size);
        for(decltype(ins_size) i=0; i<ins_size; i++){
            get_gate_params(id_tmp, name_tmp, width_tmp);
            auto &back = ins.emplace_back(new gate_in(name_tmp, width_tmp));
            back->id = id_tmp;
        }
        elem->ins = std::move(ins);
    }
    {
        auto outs_size = get_var<std::ptrdiff_t>(it);
        std::vector<std::shared_ptr<gate_out>> outs;
        outs.reserve(outs_size);
        for(decltype(outs_size) i=0; i<outs_size; i++){
            get_gate_params(id_tmp, name_tmp, width_tmp);
            auto &back = outs.emplace_back(new gate_out(name_tmp, width_tmp));
            back->id = id_tmp;

            auto ins_size = get_var<std::ptrdiff_t>(it);
            back->ins.reserve(ins_size);
            for(decltype(ins_size) i=0; i<ins_size; i++){
                get_gate_params(id_tmp, name_tmp, width_tmp);
                auto it_back = back->ins.emplace_back(new gate_in(name_tmp, width_tmp));
                it_back->id = id_tmp;
            }
        }
    }
    auto count = get_var<std::ptrdiff_t>(it);
    if(count == 1){
    }else{
        auto meta_cast = dynamic_cast<elem_meta*>(elem.get());
        std::vector<std::unique_ptr<element>> elems_inner;
        elems_inner.reserve(count);
        for(decltype(count) i=0; i<count; i++){
            from_bin_load(it, elems_inner);
        }
        meta_cast->elements = std::move(elems_inner);
    }
    elems.emplace_back(std::move(elem));
}
public:

static std::vector<uint8_t> to_bin(const element* elem){
    using namespace sim_helpers;
    std::vector<uint8_t> result;
    push(result, elem->get_id());
    push(result, elem->name.size());
    push(result, elem->name.c_str(), elem->name.size());
    push(result, p_elem_to_type(elem));

    auto push_gate = [&result](auto gt){
        auto name = gt->get_name();
        push(result, gt->get_id());
        push(result, name.size());
        push(result, name.c_str(), name.size());
        push(result, gt->get_width());
    };
    {
        auto beg = elem->get_ins_begin();
        auto end = elem->get_ins_end();
        push(result, std::distance(beg, end));
        for(auto it = beg; it != end; it++){
            auto &gt = *it;
            push_gate(gt);
        }
    }
    {
        auto beg = elem->get_outs_begin();
        auto end = elem->get_outs_end();
        push(result, std::distance(beg, end));
        for(auto it = beg; it != end; it++){
            auto &gt = *it;
            push_gate(gt);

            auto ins_beg = gt->get_tied_begin();
            auto ins_end = gt->get_tied_end();
            push(result, std::distance(ins_beg, ins_end));
            for(auto it = ins_beg; it != ins_end; it++){
                auto &in = *it;
                push_gate(in);
            }
        }
    }
    auto meta_cast = dynamic_cast<const elem_meta*>(elem);
    if(meta_cast){
        auto beg = meta_cast->get_begin();
        auto end = meta_cast->get_end();
        auto size_subs = std::distance(beg, end);
        push(result, size_subs);
        for(auto it = beg; it != end; it++){
            auto sub_bin = to_bin(it.base()->get());
            result.reserve(result.size()+sub_bin.size());
            result.insert(result.end(), sub_bin.begin(), sub_bin.end());
        }
    }else{
        std::ptrdiff_t size_subs = 1;
        push(result, size_subs);
    }
    return result;
}

static void from_bin(std::vector<uint8_t>::const_iterator &it, std::vector<std::unique_ptr<element>>& elems){
    from_bin_load(it, elems);
    retie(elems);
}

static inline std::vector<uint8_t> load_bin(const std::filesystem::path &path){
    if(!std::filesystem::exists(path)){
        throw std::runtime_error("file doesn't exist:"+path.string());
    }
    auto file = std::ifstream(path, std::ios::in | std::ios_base::binary);
    if(!file.good()){
        throw std::runtime_error("file open error");
    }
    auto size = std::filesystem::file_size(path);
    std::vector<uint8_t> result;
    result.resize(size);
    file.readsome((char*)result.data(), size);
    file.close();
    return result;
}

static inline void save_bin(std::vector<uint8_t>::const_iterator begin,
    std::vector<uint8_t>::const_iterator end,
    const std::filesystem::path &path)
{
    std::ofstream file(path, std::ios::out | std::ios::binary);
    if(!file.good()){
        throw std::runtime_error("file create error");
    }
    std::copy(begin, end, std::ostream_iterator<uint8_t>(file));
    file.close();
}

};