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
    auto root_it = sim.root();
    auto meta_elem = std::make_unique<elem_meta>("meta1");
    auto in_elem = std::make_unique<elem_in>("in");
    auto out_elem = std::make_unique<elem_out>("out");
    in_elem->get_out(0)->tie_input(out_elem->get_in(0));
    auto meta_it = sim.emplace(root_it, std::move(meta_elem));
    sim.emplace(meta_it, std::move(in_elem));
    sim.emplace(meta_it, std::move(out_elem));

    auto in_elem2 = std::make_unique<elem_in>("in2");
    auto out_elem2 = std::make_unique<elem_out>("out2");
    auto not_elem = std::make_unique<elem_not>("not1");
    in_elem2->get_out(0)->tie_input(not_elem->get_in(0));
    not_elem->get_out(0)->tie_input((*meta_it)->get_in(0));
    (*meta_it)->get_out(0)->tie_input(out_elem2->get_in(0));
    sim.emplace(root_it, std::move(in_elem2));
    sim.emplace(root_it, std::move(out_elem2));
    sim.emplace(root_it, std::move(not_elem));
    return sim;
}

};