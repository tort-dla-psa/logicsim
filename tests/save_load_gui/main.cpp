#include "sim/sim.h"
#include "sim/file_ops.h"
#include "test-helpers.h"
#include "ui/sim_ui_glue.h"
#include <filesystem>
#include <iostream>
#include <iomanip>

int main(){
    auto path = std::filesystem::path("/tmp/sim_save_gui.sim");

    auto sim1 = test_helpers::construct_test_sim();
    auto glue = sim_ui_glue();
    size_t w = 1000, h = 1000;
    for(auto &el:sim1){
        auto parent_id = el->get_parent_id();
        auto parent_view_it = glue.find_view(parent_id);
        auto view = glue.elem_to_view(el);
        glue.add_view(parent_view_it, view);
    }

    elem_file_saver saver;
    auto json_sim_save = saver.sim_to_json(sim1.begin(), sim1.end());
    if(std::filesystem::exists(path)){
        std::filesystem::remove(path);
    }
    auto json_glue_save = saver.glue_to_json(glue.begin(), glue.end());
    nlohmann::json json_save;
    json_save["sim"] = json_sim_save;
    json_save["glue"] = json_glue_save;
    saver.save_json(json_save, path);

    auto json_load = saver.load_json(path);
    auto json_sim_load = json_load["sim"];
    auto json_glue_load = json_load["glue"];

    std::cout<< "asserting that jsons are equal...";
    assert(json_load == json_save);
    std::cout<< " done\n";

    auto sim2_tree = saver.sim_from_json(json_sim_load);
    auto glue2_tree = saver.glue_from_json(json_glue_load);
    sim sim2(std::move(sim2_tree));
    sim_ui_glue glue2(std::move(glue2_tree));
    assert(test_helpers::sims_are_equal(sim1, sim2));

    std::cout<< "asserting that glues are equal";
    auto glue1_it = glue.begin();
    auto glue1_end = glue.end();
    auto glue2_it = glue2.begin();
    auto glue2_end = glue2.end();
    auto d1 = std::distance(glue1_it, glue1_end);
    auto d2 = std::distance(glue2_it, glue2_end);
    assert(d1 == d2);
    for(;glue1_it != glue1_end && glue2_it != glue2_end;
        glue1_it++, glue2_it++)
    {
        auto &v1 = *glue1_it;
        auto &v2 = *glue2_it;
        assert(v1->name == v2->name);
        assert(v1->x == v2->x);
        assert(v1->y == v2->y);
        assert(v1->w == v2->w);
        assert(v1->h == v2->h);
        assert(v1->id == v2->id);
        assert(v1->parent_id == v2->parent_id);
        std::cout<<".";
    }
    std::cout<<"done"<<std::endl;
    return 0;
};