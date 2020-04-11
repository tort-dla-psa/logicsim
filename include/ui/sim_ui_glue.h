#pragma once
#include <vector>
#include <utility>

struct elem_view;
struct gate_view;
struct gate_view_in;
struct gate_view_out;
struct gate_connection;

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
    size_t bit_width;
    std::shared_ptr<elem_view> parent;

    gate_view(){}
    virtual ~gate_view(){}
};
struct gate_view_in:gate_view{
    std::vector<std::shared_ptr<gate_connection>> conn;

    gate_view_in():gate_view(){
        this->dir = gate_view::direction::in;
    }
};
struct gate_view_out:gate_view{
    std::vector<std::shared_ptr<gate_connection>> conn;

    gate_view_out():gate_view(){
        this->dir = gate_view::direction::out;
    }
};
struct gate_connection{
    std::shared_ptr<gate_view_out> gate_out;
    std::shared_ptr<gate_view_in> gate_in;
    bool valid;

    gate_connection(std::shared_ptr<gate_view_out> gate_out,
        std::shared_ptr<gate_view_in> gate_in,
        bool valid)
        :gate_out(gate_out),
        gate_in(gate_in),
        valid(valid)
    {}
    void check_valid(){
        valid = (gate_out->bit_width == gate_in->bit_width);
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

    long bit_width; //for type_in and type_out
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

    auto find_view(const size_t &id){
        auto it = std::find_if(finder.begin(), finder.end(),
            [&id](const auto &v){
                return v->id == id;
            });
        if(it != finder.end()){
            return it;
        }
        auto mes = "No element with id "+std::to_string(id)+" in sim_glue";
        throw std::runtime_error(mes);
    }
    auto find_view(const size_t &id)const{
        auto it = std::find_if(finder.begin(), finder.end(),
            [&id](const auto &v){
                return v->id == id;
            });
        if(it != finder.end()){
            return it;
        }
        auto mes = "No element with id "+std::to_string(id)+" in sim_glue";
        throw std::runtime_error(mes);
    }

    auto find_views(const long &x, const long &y)const{
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

    auto find_views(const long &x, const long &y,
        const long &w, const long &h)const
    {
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

    const auto& access_views()const{
        return finder;
    }

    void add_view(const std::shared_ptr<elem_view> &view){
        try{
            auto it = find_view(view->id);
            *it = view;
        }catch(std::runtime_error &e){ //not found
            finder.emplace_back(view);
        }
    }

    void del_view(const size_t &id){
        auto it = find_view(id);
        auto el = *it;
        for(auto &gt_in:el->gates_in){
            for(auto &gt_in_conn:gt_in->conn){
                auto out = gt_in_conn->gate_out;
                auto conn = std::find(out->conn.begin(), out->conn.end(), gt_in_conn);
                out->conn.erase(conn);
            }
            gt_in->conn.clear();
        }
        el->gates_in.clear();
        for(auto &gt_out:el->gates_out){
            for(auto &gt_out_conn:gt_out->conn){
                auto in = gt_out_conn->gate_in;
                auto conn = std::find(in->conn.begin(), in->conn.end(), gt_out_conn);
                in->conn.erase(conn);
            }
            gt_out->conn.clear();
        }
        el->gates_out.clear();
        finder.erase(it);
    }

    auto get_gate(const std::shared_ptr<elem_view>& view, int x, int y){
        std::shared_ptr<gate_view> gt;
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
                gt = gate;
                return gt;
            } 
        }
        for(auto &gate:gates_out){
            if(gate->x-gate->w/2 <= x &&
                gate->y-gate->h/2 <= y &&
                gate->x+gate->w/2 > x &&
                gate->y+gate->h/2 > y)
            {
                gt = gate;
                return gt;
            } 
        }
        return gt; //nullptr
    }

    auto get_gates(int x, int y)const{
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