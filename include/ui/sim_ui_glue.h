#pragma once
#include <vector>
#include <utility>

struct gate_view{
    enum class direction{
        in,
        out
    }dir;

    gate_view(gate_view::direction dir)
        :dir(dir)
    {}
};
struct elem_view{
    long x, y, w=10, h=10;
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

    std::vector<elem_view> find_elements(long x, long y){
        std::vector<elem_view> ids;
        for(auto &view:finder){
            if(view.x+view.w < x &&
                view.x >= x &&
                view.y+view.h < y &&
                view.y >= y)
            {
                ids.emplace_back(view);
            }
        }
        return ids;
    }

    std::vector<elem_view> find_elements(long x, long y, long w, long h){
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

    void add_element(const elem_view &view){
        finder.emplace_back(view);
    }
};