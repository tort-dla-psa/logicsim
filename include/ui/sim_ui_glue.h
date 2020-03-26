#pragma once
#include <vector>
#include <utility>

struct gate_view{
    enum class direction{
        in,
        out
    }dir;
    size_t id;
    long x, y, w, h;

    gate_view(){}
    gate_view(gate_view::direction dir)
        :dir(dir)
    {}
};
struct elem_view{
    long x, y, w, h;
    size_t id;
    enum class type{
        type_and,
        type_or,
        type_not,
        type_custom
    }t = type::type_custom;
    enum class direction{
        dir_up,
        dir_right,
        dir_down,
        dir_left
    }dir = direction::dir_right;
    std::vector<gate_view> gates_in,
        gates_out;
};

class sim_ui_glue{
private:
    std::vector<elem_view> finder;
    sim_ui_glue(){};
public:
    static sim_ui_glue& get_instance(){
        static std::unique_ptr<sim_ui_glue> _singleton(new sim_ui_glue());
        return *_singleton;
    }

    std::vector<elem_view> find_views(long x, long y){
        std::vector<elem_view> ids;
        for(auto &view:finder){
            if(view.x+view.w > x &&
                view.x <= x &&
                view.y+view.h > y &&
                view.y <= y)
            {
                ids.emplace_back(view);
            }
        }
        return ids;
    }

    std::vector<elem_view> find_views(long x, long y, long w, long h){
        std::vector<elem_view> ids;
        for(auto &view:finder){
            if(view.x >= x &&
                view.x < x+w &&
                view.y >= y &&
                view.y < y+h)
            {
                ids.emplace_back(view);
            }
        }
        return ids;
    }

    const std::vector<elem_view>& access_views()const{
        return finder;
    }

    void add_view(const elem_view &view){
        auto it = std::find_if(finder.begin(), finder.end(),
            [&view](const auto &v){
                return v.id == view.id;
            });
        if(it != finder.end()){
            *it = view;
        }else{
            finder.emplace_back(view);
        }
    }

    std::optional<gate_view> get_gate(const elem_view& view, int x, int y){
        std::optional<gate_view> gate_opt;
        x -= view.x;
        y -= view.y;
        auto &gates_in = view.gates_in;
        auto &gates_out = view.gates_out;
        for(auto &gate:gates_in){
            if(gate.x-gate.w/2 <= x &&
                gate.y-gate.h/2 <= y &&
                gate.x+gate.w/2 > x &&
                gate.y+gate.h/2 > y)
            {
                gate_opt = gate;
                return gate_opt;
            } 
        }
        for(auto &gate:gates_out){
            if(gate.x-gate.w/2 <= x &&
                gate.y-gate.h/2 <= y &&
                gate.x+gate.w/2 > x &&
                gate.y+gate.h/2 > y)
            {
                gate_opt = gate;
                return gate_opt;
            } 
        }
        return gate_opt;
    }

    std::vector<gate_view> get_gates(int x, int y){
        std::vector<gate_view> gates;
        for(auto &view:this->finder){
            int x_ =  x - view.x;
            int y_ =  y - view.y;
            auto &gates_in = view.gates_in;
            auto &gates_out = view.gates_out;
            for(auto &gate:gates_in){
                if(gate.x-gate.w/2 <= x_ &&
                    gate.y-gate.h/2 <= y_ &&
                    gate.x+gate.w/2 > x_ &&
                    gate.y+gate.h/2 > y_)
                {
                    gates.emplace_back(gate);
                } 
            }
            for(auto &gate:gates_out){
                if(gate.x-gate.w/2 <= x_ &&
                    gate.y-gate.h/2 <= y_ &&
                    gate.x+gate.w/2 > x_ &&
                    gate.y+gate.h/2 > y_)
                {
                    gates.emplace_back(gate);
                } 
            }
        }
        return gates;
    }
};