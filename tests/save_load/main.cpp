#include "sim/sim.h"
#include "sim/file_ops.h"
#include <filesystem>
#include <iostream>
#include <iomanip>

int main(){
    auto path = std::filesystem::path("/tmp/sim_save.sim");

    class sim sim1;
    auto root1 = sim1.root();
    auto meta_elem = std::make_unique<elem_meta>("meta1");
    auto meta_it = sim1.emplace(root1, std::move(meta_elem));

    auto and_elem = std::make_unique<elem_and>("and1");
    auto not_elem = std::make_unique<elem_not>("not1");
    and_elem->get_out(0)->tie_input(not_elem->get_in(0));
    sim1.emplace(meta_it, std::move(and_elem));
    sim1.emplace(meta_it, std::move(not_elem));

    auto or_elem = std::make_unique<elem_or>("or1");
    sim1.emplace(root1, std::move(or_elem));

    elem_file_saver saver;
    auto json_save = saver.to_json(sim1.begin(), sim1.end());
    if(std::filesystem::exists(path)){
        std::filesystem::remove(path);
    }
    saver.save_json(json_save, path);

    auto json_load = saver.load_json(path);

    std::cout<< "asserting that jsons are equal...";
    assert(json_load == json_save);
    std::cout<< " done\n";

    auto sim2 = saver.from_json(json_load);

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