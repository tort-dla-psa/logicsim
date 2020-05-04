#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "meta_element.h"
#include "element.h"
#include "basic_elements.h"
#include "file_ops.h"
#include "k_tree.h"

class sim{
public:
    using k_tree_ = tree_ns::k_tree<std::unique_ptr<element>>;
    using k_tree_it = k_tree_::depth_first_node_first_iterator;

private:
    k_tree_ elems;
public:
    sim(k_tree_::value_type&& root){
        elems.set_root(std::move(root));
    }

    sim(k_tree_&& tree){
        elems = std::move(tree);
    }

    sim():
        sim(std::make_unique<elem_meta>("root"))
    {}

    inline auto root(){
        return elems.root();
    }

    inline auto begin(){
        return elems.depth_first_node_first_begin();
    }

    inline auto end(){
        return elems.depth_first_node_first_end();
    }

    inline void tick(){
        for(auto &el:elems){
            el->reset_processed();
        }
        for(auto &el:elems){
            el->process();
        }
    }

    inline k_tree_it get_by_id(const size_t &id){
        return get_by_id(elems.begin(), elems.end(), id);
    }

    inline k_tree_it get_by_id(const k_tree_it& begin, const k_tree_it& end, const size_t &id){
        return get_by_predicate(begin, end,
            [&id](auto &el){
            return el->get_id() == id;
        });
    }

    inline k_tree_it get_by_predicate(std::function<bool(const k_tree_::value_type&)> predicate){
        return std::find_if(elems.begin(), elems.end(), predicate);
    }

    inline k_tree_it get_by_predicate(const k_tree_it& begin, const k_tree_it& end,
        std::function<bool(const k_tree_::value_type&)> predicate)
    {
        return std::find_if(begin, end, predicate);
    }

    template<class It>
    It erase(It it){
        return elems.erase(it);
    }

    inline k_tree_it emplace(k_tree_::value_type&& val){
        auto it = elems.root();
        return emplace(it, std::move(val));
    }

    inline k_tree_it emplace(const k_tree_it& it, k_tree_::value_type&& val){
        auto &el = (*it);
        val->parent_id = el->get_id();
        if(dynamic_cast<elem_meta*>(el.get())){
            auto el_in = dynamic_cast<elem_in*>(val.get());
            auto el_out = dynamic_cast<elem_out*>(val.get());
            if(el_in){
                auto outer = el_in->gt_outer;
                el->emplace_back(outer);
                //insert gate in left-most position to tick as early as possible
                return elems.child_prepend(it, std::move(val));
            }else if(el_out){
                auto outer = el_out->gt_outer;
                el->emplace_back(outer);
            }
        }
        return elems.child_append(it, std::move(val));
    }
};