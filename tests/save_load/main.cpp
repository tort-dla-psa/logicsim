#include "nlohmann/json.hpp"
#include "sim/sim.h"
#include "test-helpers.h"
#include "sim/file_ops.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

int main(){
    auto path = std::filesystem::path("/tmp/sim_save.sim");
    auto sim1 = test_helpers::construct_test_sim();

    nlohmann::json j;
    sim1.to_json(j);
    if(std::filesystem::exists(path)){
        std::filesystem::remove(path);
    }

    std::ofstream os(path);
    os << j;
    os.close();

    j.clear();

    std::ifstream is(path);
    is >> j;
    is.close();

    sim sim2;
    sim2.from_json(j);

    if(!test_helpers::sims_are_equal(sim1, sim2)){
        return -1;
    }else{
        return 0;
    }
};
