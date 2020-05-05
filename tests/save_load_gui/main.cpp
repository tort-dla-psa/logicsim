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
    auto glue = sim_ui_glue::get_instance();
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
    saver.save_json(json_sim_save, path);

    auto json_sim_load = saver.load_json(path);

    std::cout<< "asserting that jsons are equal...";
    assert(json_sim_load == json_sim_save);
    std::cout<< " done\n";

    auto sim2 = saver.sim_from_json(json_sim_load);
    //auto glue2 = saver.glue_from_json(json_load);

    auto assert_nameable = [](const auto &el1, const auto &el2){
        assert(el1->get_name() == el2->get_name());
        assert(el1->get_id() == el2->get_id());
        assert(el1->get_parent_id() == el2->get_parent_id());
    };

    std::cout<< "asserting that elements in trees are equal";
    auto sim1_it = sim1.begin();
    auto sim1_it_end = sim1.end();
    auto d1 = std::distance(sim1_it, sim1_it_end);
    auto sim2_it = sim2.begin();
    auto sim2_it_end = sim2.end();
    auto d2 = std::distance(sim2_it, sim2_it_end);
    assert(d1 == d2);
    for(;sim1_it != sim1_it_end && sim2_it != sim2_it_end;
        sim1_it++, sim2_it++)
    {
        const auto &el1 = *sim1_it;
        const auto &el2 = *sim2_it;
        assert_nameable(el1, el2);

        {
            auto el1_in_it = el1->get_ins_begin();
            auto el2_in_it = el2->get_ins_begin();
            const auto el1_in_it_end = el1->get_ins_end();
            const auto el2_in_it_end = el2->get_ins_end();
            d1 = std::distance(el1_in_it, el1_in_it_end);
            d2 = std::distance(el2_in_it, el2_in_it_end);
            assert(d1 == d2);
                
            for(;el1_in_it != el1_in_it_end && el2_in_it != el1_in_it_end;
                el1_in_it++, el2_in_it++)
            {
                auto &in1 = *el1_in_it;
                auto &in2 = *el2_in_it;
                assert_nameable(in1, in2);
            }
        }

        {
            auto el1_out_it = el1->get_outs_begin();
            auto el2_out_it = el2->get_outs_begin();
            const auto el1_out_it_end = el1->get_outs_end();
            const auto el2_out_it_end = el2->get_outs_end();
            d1 = std::distance(el1_out_it, el1_out_it_end);
            d2 = std::distance(el2_out_it, el2_out_it_end);
            assert(d1 == d2);
            for(;el1_out_it != el1_out_it_end && el2_out_it != el2_out_it_end;
                el1_out_it++, el2_out_it++)
            {
                auto &out1 = *el1_out_it;
                auto &out2 = *el2_out_it;
                assert_nameable(out1, out2);

                auto out1_in = out1->get_tied_begin();
                auto out1_in_end = out1->get_tied_end();
                auto out2_in = out2->get_tied_begin();
                auto out2_in_end = out2->get_tied_end();
                d1 = std::distance(out1_in, out1_in_end);
                d2 = std::distance(out2_in, out2_in_end);
                assert(d1 == d2);
                for(;out1_in != out1_in_end && out2_in != out2_in_end;
                    out1_in++, out2_in++)
                {
                    auto &gt1 = *out1_in;
                    auto &gt2 = *out2_in;
                    assert_nameable(gt1, gt2);
                }
            }
        }
        std::cout<<".";
    }
    std::cout<<" done\n";
};