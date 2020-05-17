#include "sim/sim.h"
#include "test-helpers.h"
#include "sim/file_ops.h"
#include <filesystem>
#include <iostream>
#include <iomanip>

int main(){
    auto path = std::filesystem::path("/tmp/sim_save.sim");
    auto sim1 = test_helpers::construct_test_sim();

    elem_file_saver saver;
    auto json_save = saver.sim_to_json(sim1.begin(), sim1.end());
    if(std::filesystem::exists(path)){
        std::filesystem::remove(path);
    }
    saver.save_json(json_save, path);

    auto json_load = saver.load_json(path);

    std::cout<< "asserting that jsons are equal...";
    assert(json_load == json_save);
    std::cout<< " done\n";

    auto sim2_tree = saver.sim_from_json(json_load);
    sim sim2(std::move(sim2_tree));
    if(!test_helpers::sims_are_equal(sim1, sim2)){
        return -1;
    }else{
        return 0;
    }
};