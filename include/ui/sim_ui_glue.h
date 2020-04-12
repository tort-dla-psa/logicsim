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
    
    enum class state{
        normal,
        creating,
        selected,
        copied,
        cut
    }st;
};
struct gate_view:view{
    enum class direction{
        in,
        out
    }dir;
    size_t bit_width;
    std::shared_ptr<elem_view> parent;
    std::vector<std::shared_ptr<gate_connection>> conn;

    gate_view(){}
    virtual ~gate_view(){}
};
struct gate_view_in:gate_view{
    gate_view_in():gate_view(){
        this->dir = gate_view::direction::in;
    }
};
struct gate_view_out:gate_view{
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
    std::vector<std::shared_ptr<elem_view>> views;
    sim_ui_glue(){};

    auto prv_find_view_impl(const size_t &id)const{
        auto it = std::find_if(views.cbegin(), views.cend(),
            [&id](const auto &v){
                return v->id == id;
            });
        if(it != views.cend()){
            return it;
        }
        auto mes = "No element with id "+std::to_string(id)+" in sim_glue";
        throw std::runtime_error(mes);
    }
public:
    static sim_ui_glue& get_instance(){
        static std::unique_ptr<sim_ui_glue> _singleton(new sim_ui_glue());
        return *_singleton;
    }

    auto find_view(const size_t &id){
        auto const_it = prv_find_view_impl(id);
        auto dist = std::distance(views.cbegin(), const_it);
        return views.begin()+dist;
    }
    auto find_view(const size_t &id)const{
        return prv_find_view_impl(id);
    }

    auto find_views(const long &x, const long &y)const{
        std::vector<std::shared_ptr<elem_view>> ids;
        for(auto &view:views){
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
        auto predicate = [&x, &y, &w, &h](std::shared_ptr<elem_view> view){
            return view->x >= x &&
                view->x < x+w &&
                view->y >= y &&
                view->y < y+h;
        };
        std::copy_if(views.begin(), views.end(), std::back_inserter(ids), predicate);
        return ids;
    }

    const auto& access_views()const{
        return views;
    }

    void add_view(const std::shared_ptr<elem_view> &view){
        try{
            auto it = find_view(view->id);
            *it = view;
        }catch(std::runtime_error &e){ //not found
            views.emplace_back(view);
        }
    }

    void del_view(const size_t &id){
        auto it = find_view(id);
        auto el = *it;
        for(auto &gt_in:el->gates_in){
            for(auto &gt_in_conn:gt_in->conn){
                auto out = gt_in_conn->gate_out;
                //std::remove(out->conn.begin(), out->conn.end(), gt_in_conn);
                auto it = std::find(out->conn.begin(), out->conn.end(), gt_in_conn);
                out->conn.erase(it);
            }
            gt_in->conn.clear();
        }
        el->gates_in.clear();
        for(auto &gt_out:el->gates_out){
            for(auto &gt_out_conn:gt_out->conn){
                auto in = gt_out_conn->gate_in;
                //std::remove(in->conn.begin(), in->conn.end(), gt_out_conn);
                auto it = std::find(in->conn.begin(), in->conn.end(), gt_out_conn);
                in->conn.erase(it);
            }
            gt_out->conn.clear();
        }
        el->gates_out.clear();
        views.erase(it);
    }

    auto get_gate(const std::shared_ptr<elem_view>& view, int x, int y)const{
        std::shared_ptr<gate_view> gt;
        x -= view->x;
        y -= view->y;
        auto &gates_in = view->gates_in;
        auto &gates_out = view->gates_out;
        auto predicate = [&x, &y](std::shared_ptr<gate_view> gate){
            return gate->x-gate->w/2 <= x &&
                gate->y-gate->h/2 <= y &&
                gate->x+gate->w/2 > x &&
                gate->y+gate->h/2 > y;
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
        auto predicate = [](std::shared_ptr<gate_view> gate, int x_, int y_){
            return gate->x <= x_ &&
                gate->y <= y_ &&
                gate->x+gate->w > x_ &&
                gate->y+gate->h > y_;
        };
        for(auto &view:this->views){
            int x_ =  x - view->x;
            int y_ =  y - view->y;
            auto &gates_in = view->gates_in;
            auto &gates_out = view->gates_out;
            auto tmp_predicate = [&x_, &y_, &predicate](std::shared_ptr<gate_view> ptr){
                return predicate(ptr, x_, y_);
            };
            std::copy_if(gates_in.begin(), gates_in.end(), std::back_inserter(gates), tmp_predicate);
            std::copy_if(gates_out.begin(), gates_out.end(), std::back_inserter(gates), tmp_predicate);
        }
        return gates;
    }

    void tie_gates(std::shared_ptr<gate_view> gate_view_1, std::shared_ptr<gate_view> gate_view_2, bool valid){
        auto tie = [&valid](auto cast_in, auto cast_out){
            auto conn = std::make_shared<gate_connection>(cast_out, cast_in, valid);
            cast_out->conn.emplace_back(conn);
            cast_in->conn.emplace_back(conn);
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
};