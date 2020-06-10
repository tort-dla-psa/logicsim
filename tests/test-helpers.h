#pragma once
#include "sim/sim.h"

namespace test_helpers{

/*
Constructs this thing for tests:
                    +--------------+           
                    |     META     |           
+----+  +-----+  +--+-----+  +-----+--+ +-----+
| IN ---> NOT --->  | IN  ---> OUT |  --->OUT |
+----+  +-----+  +--+-----+  +-----+--+ +-----+
                    +--------------+           
*/
inline auto construct_test_sim(){
    class sim sim;
    auto root_it = sim.begin();
    auto meta_elem = std::make_unique<elem_meta>("meta1");
    auto in_elem = std::make_unique<elem_in>("in");
    auto out_elem = std::make_unique<elem_out>("out");
    in_elem->out(0)->tie_input(out_elem->in(0));
    auto meta_it = sim.emplace(root_it, std::move(meta_elem));
    sim.emplace(meta_it, std::move(in_elem));
    sim.emplace(meta_it, std::move(out_elem));

    auto in_elem2 = std::make_unique<elem_in>("in2");
    auto out_elem2 = std::make_unique<elem_out>("out2");
    auto not_elem = std::make_unique<elem_not>("not1");
    in_elem2->out(0)->tie_input(not_elem->in(0));
    not_elem->out(0)->tie_input((*meta_it)->in(0));
    (*meta_it)->out(0)->tie_input(out_elem2->in(0));
    sim.emplace(root_it, std::move(in_elem2));
    sim.emplace(root_it, std::move(out_elem2));
    sim.emplace(root_it, std::move(not_elem));
    return sim;
}

bool sims_are_equal(sim &sim1, sim &sim2){
    auto assert_nameable = [](const auto &el1, const auto &el2){
        bool status = (el1->name() == el2->name()) &&
            (el1->id() == el2->id()) &&
            (el1->parent_id() == el2->parent_id());
        if(!status){
            std::cerr<<"nameable "<<el1->name()<<" != "<<el2->name()<<"\n";
        }
        return status;
    };

    std::cout<< "asserting that elements in trees are equal:";
    auto sim1_it = sim1.begin();
    auto sim1_it_end = sim1.end();
    auto d1 = std::distance(sim1_it, sim1_it_end);
    auto sim2_it = sim2.begin();
    auto sim2_it_end = sim2.end();
    auto d2 = std::distance(sim2_it, sim2_it_end);
    if(d1 != d2){
        std::cerr<<"d1 of sim1 != d2 of sim2\n";
        return false;
    }
    for(;sim1_it != sim1_it_end && sim2_it != sim2_it_end;
        sim1_it++, sim2_it++)
    {
        const auto &el1 = *sim1_it;
        const auto &el2 = *sim2_it;
        if(!assert_nameable(el1, el2)){
            return false;
        }

        {
            auto el1_in_it = el1->ins_begin();
            auto el2_in_it = el2->ins_begin();
            const auto el1_in_it_end = el1->ins_end();
            const auto el2_in_it_end = el2->ins_end();
            d1 = std::distance(el1_in_it, el1_in_it_end);
            d2 = std::distance(el2_in_it, el2_in_it_end);
            if(d1 != d2){
                std::cerr<<"d1 of ins of "<<el1->name()
                    <<" != d2 of ins of "<<el2->name()<<"\n";
                return false;
            }
                
            for(;el1_in_it != el1_in_it_end && el2_in_it != el1_in_it_end;
                el1_in_it++, el2_in_it++)
            {
                auto &in1 = *el1_in_it;
                auto &in2 = *el2_in_it;
                if(!assert_nameable(in1, in2)){
                    return false;
                }
            }
        }

        {
            auto el1_out_it = el1->outs_begin();
            auto el2_out_it = el2->outs_begin();
            const auto el1_out_it_end = el1->outs_end();
            const auto el2_out_it_end = el2->outs_end();
            d1 = std::distance(el1_out_it, el1_out_it_end);
            d2 = std::distance(el2_out_it, el2_out_it_end);
            if(d1 != d2){
                std::cerr<<"d1 of outs of "<<el1->name()
                    <<" != d2 of outs of "<<el2->name()<<"\n";
            }
            for(;el1_out_it != el1_out_it_end && el2_out_it != el2_out_it_end;
                el1_out_it++, el2_out_it++)
            {
                auto &out1 = *el1_out_it;
                auto &out2 = *el2_out_it;
                if(!assert_nameable(out1, out2)){
                    return false;
                }

                auto out1_in = out1->tied_begin();
                auto out1_in_end = out1->tied_end();
                auto out2_in = out2->tied_begin();
                auto out2_in_end = out2->tied_end();
                d1 = std::distance(out1_in, out1_in_end);
                d2 = std::distance(out2_in, out2_in_end);
                if(d1 != d2){
                    std::cerr<<"d1 of connected ins of "<<out1->name()<<"::"<<el1->name()
                        <<" != d2 of connected ins of "<<out2->name()<<"::"<<el2->name()<<"\n";
                    return false;
                }
                for(;out1_in != out1_in_end && out2_in != out2_in_end;
                    out1_in++, out2_in++)
                {
                    auto &gt1 = *out1_in;
                    auto &gt2 = *out2_in;
                    if(!assert_nameable(gt1, gt2)){
                        return false;
                    }
                }
            }
        }
        std::cout<<".";
    }
    std::cout<<"done"<<std::endl;
    return true;
};
};
