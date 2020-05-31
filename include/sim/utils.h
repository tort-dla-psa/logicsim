#pragma once
#include "basic_elements.h"
#include <nlohmann/json.hpp>

namespace utils{
    element* from_json_el(const nlohmann::json &j);
    element* from_json_gt(const nlohmann::json &j);
};