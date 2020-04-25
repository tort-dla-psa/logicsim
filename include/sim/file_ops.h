#pragma once
#include <fstream>
#include <iterator>
#include <filesystem>
#include <vector>
#include "meta_element.h"
#include "basic_elements.h"
#include "helpers.h"

class elem_file_saver{
public:

static std::vector<uint8_t> to_bin(const element* elem){
    using namespace sim_helpers;
    std::vector<uint8_t> result;
    push(result, elem->get_id());
    auto push_gate = [&result](auto gt){
        auto name = gt->get_name();
        push(result, gt->get_id());
        push(result, name.size());
        push(result, name);
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
        for(auto it = beg; it != end; it++){
            auto sub_bin = to_bin(it.base()->get());
            result.reserve(result.size()+sub_bin.size());
            result.insert(result.end(), sub_bin.begin(), sub_bin.end());
        }
    }else{
        std::ptrdiff_t size_subs = 1;
        auto bytes = reinterpret_cast<const char*>(&size_subs);
        result.insert(result.end(), bytes, bytes+sizeof(size_subs));
    }
    return result;
}

static void from_bin(std::vector<uint8_t>::const_iterator &it, element*& elem){
    using namespace sim_helpers;
    elem->id = get_var<decltype(elem->id)>(it);
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
}

};

inline std::vector<uint8_t> load_bin(const std::filesystem::path &path){
    if(!std::filesystem::exists(path)){
        throw std::runtime_error("file doesn't exists");
    }
    auto file = std::ifstream(path, std::ios::in | std::ios::binary);
    if(!file.good()){
        throw std::runtime_error("file open error");
    }
    auto size = std::filesystem::file_size(path);
    std::vector<uint8_t> result;
    result.reserve(size);
    result.insert(result.end(),
        std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>());
    file.close();
    return result;
}

inline void save_bin(std::vector<uint8_t>::const_iterator begin,
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