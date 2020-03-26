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
};