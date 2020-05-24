#pragma once
#include <vector>
#include <utility>
#include "nlohmann/json.hpp"
#include "k_tree.hpp"
#include "sim/basic_elements.h"
#include "sim/meta_element.h"

struct elem_view;
struct gate_view;
struct gate_view_in;
struct gate_view_out;
struct gate_connection;

struct view{
    std::string name = "generic_name";
    size_t id, parent_id = -1;
    long x, y, w, h;
    
    enum class state{
        normal,
        creating,
        selected,
        copied,
        cut
    }st;

    view(){};
    virtual ~view(){}
};

struct gate_view:view{
    enum class direction{
        in,
        out
    }dir;
    size_t bit_width=1;
    int value=0;

    gate_view(){
        this->w = 10;
        this->h = 10;
    }
    virtual ~gate_view(){}
};
struct gate_view_in:gate_view{
    gate_view_in():gate_view(){
        this->dir = gate_view::direction::in;
    }
};
struct gate_view_out:gate_view{
    std::vector<std::shared_ptr<gate_view_in>> ins;
    gate_view_out():gate_view(){
        this->dir = gate_view::direction::out;
    }
};
struct elem_view:view{
    enum class direction{
        dir_up,
        dir_right,
        dir_down,
        dir_left
    }dir = direction::dir_right;

    elem_view(){
        this->w = 50;
        this->h = 50;
    }

    std::vector<std::shared_ptr<gate_view_in>> ins;
    std::vector<std::shared_ptr<gate_view_out>> outs;
};

struct elem_view_and:elem_view{};
struct elem_view_nand:elem_view{};
struct elem_view_or:elem_view{};
struct elem_view_nor:elem_view{};
struct elem_view_xor:elem_view{};
struct elem_view_xnor:elem_view{};
struct elem_view_not:elem_view{};
struct elem_view_gate:elem_view{};
struct elem_view_in:elem_view_gate{
    std::shared_ptr<gate_view_in> gt_outer;
};
struct elem_view_out:elem_view_gate{
    std::shared_ptr<gate_view_out> gt_outer;
};
struct elem_view_meta:elem_view{};

class sim_ui_glue{
private:
    using k_tree_ = k_tree::tree<std::shared_ptr<elem_view>>;
    using k_tree_it = k_tree_::depth_first_iterator;

    k_tree_ tree;
    std::shared_ptr<elem_view_meta> global_root, root;
public:
    sim_ui_glue(){
        this->global_root = std::make_shared<elem_view_meta>();
        this->global_root->id = 0;
        this->global_root->name = "root";
        this->root = global_root;
        tree.set_root(root);
    };

    sim_ui_glue(const k_tree_ &tree){
        this->tree = tree;
        auto root_tree_ptr = *std::find_if(tree.begin(), tree.end(),
            [](const auto &el){
                return el->id == 0;
        });
        this->global_root = std::dynamic_pointer_cast<elem_view_meta>(root_tree_ptr);
        this->root = global_root;
    }

    void set_root(const std::shared_ptr<elem_view> view){
        auto cast = std::dynamic_pointer_cast<elem_view_meta>(view);
        if(!cast && view != nullptr){
            auto mes = "setting root not to element that is not meta_element";
            throw std::runtime_error(mes);
        }
        this->root = cast;
        if(!this->root){
            this->root = global_root;
        }
    }

    bool root_is_not_global(){
        return root != global_root;
    }
    void go_to_global_root(){
        set_root(global_root);
    }

    auto get_root()const{
        return root;
    }

    auto find_view(const size_t &id){
        auto predicate = [&id](auto el){
            return el->id == id;
        };
        return std::find_if(tree.begin(), tree.end(), predicate);
    }

    auto find_views(const std::function<bool(const std::shared_ptr<elem_view>&)> &predicate)const{
        std::vector<std::shared_ptr<elem_view>> result;
        std::copy_if(tree.begin(), tree.end(), std::back_inserter(result), predicate);
        return result;
    }

    auto find_views(const long &x, const long &y)const{
        auto predicate = [&x, &y, this](auto view){
            return (view->x+view->w > x &&
                view->x <= x &&
                view->y+view->h > y &&
                view->y <= y &&
                view->parent_id == root->id);
        };
        return find_views(predicate);
    }

    auto find_views(long x, long y, long w, long h)const{
        x = std::min(x, x+w);
        y = std::min(y, y+h);
        w = std::abs(w);
        h = std::abs(h);
        auto predicate = [&x, &y, &w, &h, this](auto view){
            return view->x >= x &&
                view->x < x+w &&
                view->y >= y &&
                view->y < y+h &&
                view->parent_id == root->id;
        };
        return find_views(predicate);
    }

    k_tree_it add_view(std::shared_ptr<elem_view> view){
        auto view_it = find_view(view->id);
        if(view_it != tree.end()){
            *view_it = view;
            return view_it;
        }
        auto root_it = find_view(root->id);
        return add_view(root_it, view);
    }

    k_tree_it add_view(k_tree_it it, std::shared_ptr<elem_view> &view){
        auto meta_cast = std::dynamic_pointer_cast<elem_view_meta>(*it);
        if(!meta_cast){
            throw std::runtime_error("attempt to add sub-element to an element that is not meta");
        }
        view->parent_id = meta_cast->id;
        return tree.append_child(it, view);
    }

    void del_view(const size_t &id){
        auto it = find_view(id);
        auto el = *it;
        for(auto el_:tree){
            if(el_ == el){
                continue;
            }
            //ISSUE:too complicated. we could search only through level of "el"
            for(auto out:el_->outs){
                auto in_it_ = out->ins.begin();
                while(in_it_!= out->ins.end()){
                    for(auto in:el->ins){
                        if(*in_it_ == in){
                            in_it_ = out->ins.erase(in_it_);
                        }else{
                            in_it_++;
                        }
                    }
                }
            }
        }
        el->ins.clear();
        for(auto &out:el->outs){
            out->ins.clear();
        }
        el->outs.clear();
        tree.erase(it);
    }

    auto get_gate(const std::shared_ptr<elem_view>& view, int x, int y)const{
        std::shared_ptr<gate_view> gt;
        x -= view->x;
        y -= view->y;
        auto &gates_in = view->ins;
        auto &gates_out = view->outs;
        auto predicate = [&x, &y, &view](auto gate){
            return gate->x-gate->w/2 <= x &&
                gate->y-gate->h/2 <= y &&
                gate->x+gate->w/2 > x &&
                gate->y+gate->h/2 > y &&
                gate->parent_id == view->id;
        };
        auto it_in = std::find_if(gates_in.begin(), gates_in.end(), predicate);
        if(it_in != gates_in.end()){
            gt = *it_in;
            return gt;
        }
        auto it_out = std::find_if(gates_out.begin(), gates_out.end(), predicate);
        if(it_out != gates_out.end()){
            gt = *it_out;
            return gt;
        }
        return gt; //nullptr
    }

    auto get_gates(int x, int y)const{
        std::vector<std::shared_ptr<gate_view>> gates;
        auto predicate = [](auto gate, int x_, int y_){
            return gate->x <= x_ &&
                gate->y <= y_ &&
                gate->x+gate->w > x_ &&
                gate->y+gate->h > y_;
        };
        for(auto &view:tree){
            if(view->parent_id != root->id){
                continue;
            }
            int x_ =  x - view->x;
            int y_ =  y - view->y;
            auto elem = std::dynamic_pointer_cast<elem_view>(view);
            auto &gates_in = elem->ins;
            auto &gates_out = elem->outs;
            auto tmp_predicate = [&x_, &y_, &predicate](auto ptr){
                return predicate(ptr, x_, y_);
            };
            std::copy_if(gates_in.begin(), gates_in.end(), std::back_inserter(gates), tmp_predicate);
            std::copy_if(gates_out.begin(), gates_out.end(), std::back_inserter(gates), tmp_predicate);
        }
        return gates;
    }

    void tie_gates(std::shared_ptr<gate_view> gate_view_1, std::shared_ptr<gate_view> gate_view_2, bool valid){
        auto tie = [&valid](auto cast_in, auto cast_out){
            auto cast_in_id = cast_in->id;
            auto in_it = std::find_if(cast_out->ins.begin(), cast_out->ins.end(),
                [&cast_in_id](auto &in){
                    return in->id == cast_in_id;
            });
            if(in_it == cast_out->ins.end()){
                cast_out->ins.emplace_back(cast_in);
            }
        };
        auto cast_in = std::dynamic_pointer_cast<gate_view_in>(gate_view_1);
        auto cast_out = std::dynamic_pointer_cast<gate_view_out>(gate_view_2);
        if(cast_in && cast_out){
            tie(cast_in, cast_out);
            return;
        }
        cast_in = std::dynamic_pointer_cast<gate_view_in>(gate_view_2);
        cast_out = std::dynamic_pointer_cast<gate_view_out>(gate_view_1);
        if(cast_in && cast_out){
            tie(cast_in, cast_out);
            return;
        }
    }

    std::shared_ptr<elem_view> elem_to_view(const std::unique_ptr<element> &elem){
        std::shared_ptr<elem_view> view;
        std::shared_ptr<elem_view_in> in_cast;
        std::shared_ptr<elem_view_out> out_cast;
        elem_in* elem_in_cast;
        elem_out* elem_out_cast;
        if((elem_in_cast = dynamic_cast<class elem_in*>(elem.get()))){
            in_cast = std::make_shared<elem_view_in>();
            view = in_cast;
        }else if((elem_out_cast = dynamic_cast<class elem_out*>(elem.get()))){
            out_cast = std::make_shared<elem_view_out>();
            view = out_cast;
        }else if(dynamic_cast<class elem_and*>(elem.get())){
            view = std::make_shared<elem_view_and>();
        }else if(dynamic_cast<class elem_or*>(elem.get())){
            view = std::make_shared<elem_view_or>();
        }else if(dynamic_cast<class elem_not*>(elem.get())){
            view = std::make_shared<elem_view_not>();
        }else if(dynamic_cast<class elem_meta*>(elem.get())){
            view = std::make_shared<elem_view_meta>();
        }
        auto id = elem->get_id();

        view->name = elem->get_name();
        view->id = id;
        view->dir = elem_view::direction::dir_right;
        view->st = elem_view::state::normal;
        view->parent_id = this->get_root()->id;

        place_gates_in(view, elem);
        place_gates_out(view, elem);

        if(elem_in_cast){
            auto outer = std::make_shared<gate_view_in>();
            outer->id = elem_in_cast->get_outer_id();
            in_cast->gt_outer = outer;
        }else if(elem_out_cast){
            auto outer = std::make_shared<gate_view_out>();
            outer->id = elem_out_cast->get_outer_id();
            out_cast->gt_outer = outer;
        }
        return view;
    }

    void place_gates_in(std::shared_ptr<elem_view> &view,
        const std::unique_ptr<element> &elem)
    {
        if(elem->get_ins_size() == 0){
            return;
        }
        long ins_offset = view->h/elem->get_ins_size();
        long ins_x = 0;
        long ins_y = ins_offset/2;
        auto gt_ = std::make_shared<gate_view_in>();
        gt_->x = ins_x;
        gt_->parent_id = view->id;
        for(size_t i=0; i<elem->get_ins_size(); i++){
            auto gt = std::make_shared<gate_view_in>(*gt_);
            gt->y = ins_y;
            ins_y += ins_offset;
            auto gt_in = elem->get_in(i);
            gt->id = gt_in->get_id();
            gt->name = gt_in->get_name();
            gt->bit_width = gt_in->get_width();
            view->ins.emplace_back(gt);
        }
    }

    void place_gates_out(std::shared_ptr<elem_view> &view,
        const std::unique_ptr<element> &elem)
    {
        if(elem->get_outs_size() == 0){
            return;
        }
        long outs_offset = view->h/elem->get_outs_size();
        long outs_x = view->w;
        long outs_y = outs_offset/2;
        auto gt_ = std::make_shared<gate_view_out>();
        gt_->x = outs_x;
        gt_->parent_id = view->id;
        for(size_t i = 0; i < elem->get_outs_size(); i++){
            auto gt = std::make_shared<gate_view_out>(*gt_);
            gt->y = outs_y;
            outs_y += outs_offset;
            auto gt_out = elem->get_out(i); 
            gt->id = gt_out->get_id();
            gt->name = gt_out->get_name();
            gt->bit_width = gt_out->get_width();
            view->outs.emplace_back(gt);
        }
    }

    auto begin(){
        return tree.begin();
    }

    auto end(){
        return tree.end();
    }
};
