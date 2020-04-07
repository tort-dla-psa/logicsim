#pragma once
#include <vector>
#include <utility>

struct elem_view;
struct gate_view;
struct gate_view_in;
struct gate_view_out;

struct view{
    std::string name;
    size_t id;
    long x, y, w, h;
};
struct gate_view:view{
    enum class direction{
        in,
        out
    }dir;
    std::shared_ptr<elem_view> parent;

    gate_view(){}
    virtual ~gate_view(){}
};

struct gate_view_in:gate_view{
    std::vector<std::shared_ptr<gate_view_out>> gates_out;

    gate_view_in():gate_view(){
        this->dir = gate_view::direction::in;
    }
};
struct gate_view_out:gate_view{
    std::vector<std::shared_ptr<gate_view_in>> gates_in;

    gate_view_out():gate_view(){
        this->dir = gate_view::direction::out;
    }
};
struct elem_view:view{
    enum class type{
        type_and,
        type_or,
        type_not,
        type_in,
        type_out,
        type_custom
    }t = type::type_custom;
    enum class direction{
        dir_up,
        dir_right,
        dir_down,
        dir_left
    }dir = direction::dir_right;
    std::vector<std::shared_ptr<gate_view_in>> gates_in;
    std::vector<std::shared_ptr<gate_view_out>> gates_out;
};

class sim_ui_glue{
private:
    std::vector<std::shared_ptr<elem_view>> finder;
    sim_ui_glue(){};
public:
    static sim_ui_glue& get_instance(){
        static std::unique_ptr<sim_ui_glue> _singleton(new sim_ui_glue());
        return *_singleton;
    }

    std::vector<std::shared_ptr<elem_view>> find_views(long x, long y){
        std::vector<std::shared_ptr<elem_view>> ids;
        for(auto &view:finder){
            if(view->x+view->w > x &&
                view->x <= x &&
                view->y+view->h > y &&
                view->y <= y)
            {
                ids.emplace_back(view);
            }
        }
        return ids;
    }

    std::vector<std::shared_ptr<elem_view>> find_views(long x, long y, long w, long h){
        std::vector<std::shared_ptr<elem_view>> ids;
        for(auto &view:finder){
            if(view->x >= x &&
                view->x < x+w &&
                view->y >= y &&
                view->y < y+h)
            {
                ids.emplace_back(view);
            }
        }
        return ids;
    }

    const std::vector<std::shared_ptr<elem_view>>& access_views()const{
        return finder;
    }

    void add_view(const std::shared_ptr<elem_view> &view){
        auto it = std::find_if(finder.begin(), finder.end(),
            [&view](const auto &v){
                return v->id == view->id;
            });
        if(it != finder.end()){
            *it = view;
        }else{
            finder.emplace_back(view);
        }
    }

    std::shared_ptr<gate_view> get_gate(const std::shared_ptr<elem_view>& view, int x, int y){
        x -= view->x;
        y -= view->y;
        auto &gates_in = view->gates_in;
        auto &gates_out = view->gates_out;
        for(auto &gate:gates_in){
            if(gate->x-gate->w/2 <= x &&
                gate->y-gate->h/2 <= y &&
                gate->x+gate->w/2 > x &&
                gate->y+gate->h/2 > y)
            {
                return gate;
            } 
        }
        for(auto &gate:gates_out){
            if(gate->x-gate->w/2 <= x &&
                gate->y-gate->h/2 <= y &&
                gate->x+gate->w/2 > x &&
                gate->y+gate->h/2 > y)
            {
                return gate;
            } 
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<gate_view>> get_gates(int x, int y){
        std::vector<std::shared_ptr<gate_view>> gates;
        for(auto &view:this->finder){
            int x_ =  x - view->x;
            int y_ =  y - view->y;
            auto &gates_in = view->gates_in;
            auto &gates_out = view->gates_out;
            for(auto &gate:gates_in){
                if(gate->x <= x_ &&
                    gate->y <= y_ &&
                    gate->x+gate->w > x_ &&
                    gate->y+gate->h > y_)
                {
                    gates.emplace_back(gate);
                } 
            }
            for(auto &gate:gates_out){
                if(gate->x <= x_ &&
                    gate->y <= y_ &&
                    gate->x+gate->w > x_ &&
                    gate->y+gate->h > y_)
                {
                    gates.emplace_back(gate);
                } 
            }
        }
        return gates;
    }
};