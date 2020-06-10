#pragma once
#include <fstream>
#include <functional>
#include <iterator>
#include <filesystem>
#include <vector>
#include "k_tree.hpp"

class elem_file_saver{
public:
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
