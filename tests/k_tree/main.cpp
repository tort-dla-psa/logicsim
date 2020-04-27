#include <iostream>
#include "sim/k_tree.h"

int main(){
    tree_ns::k_tree<int> tree;
    //construct this:
    //0-v
    //1-3-v
    //|   4
    //2
    tree.set_root(0);
    auto it_one = tree.child_append(tree.root(), 1);
    tree.child_append(it_one, 2);
    auto it_three = tree.child_append(tree.root(), 3);
    tree.child_append(it_three, 4);

    for(auto &el:tree){
        std::cout<<el<<" ";
    }
}