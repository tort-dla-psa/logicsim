#include <iostream>
#include <cassert>
#include "sim/sim.h"

int main(int argc, char* argv[]){
    assert(argc == 3);
    class sim sim;
    bool arg1 = std::atoi(argv[1]);
    bool arg2 = std::atoi(argv[2]);
    bool referense = arg1 && arg2;
    elem_and _and("and1");
    _and.get_in(0)->set_values({arg1});
    _and.get_in(1)->set_values({arg2});
    _and.process();
    bool result = _and.get_out(0)->get_value(0);
    assert(referense == result);
}