#pragma once
#include <QObject>
#include <string>
#include "sim/iserializable.h"

class view:public QObject, virtual public ISerializable{
    Q_OBJECT
public:
    enum state{
        normal,
        creating,
        selected,
        copied,
        cut
    };
protected:
    friend class gate_view_in;
    friend class gate_view_out;
    friend class sim_ui_glue;
    friend class sim_interface;
    std::string m_name = "generic_name";
    size_t m_id, m_parent_id = -1;
    long m_x, m_y, m_w, m_h;
    enum state st;
public:
    view(){}
    virtual ~view(){}

    const auto& id()const{ return m_id; }
    const auto& parent_id()const{ return m_parent_id; }
    const auto& x()const{ return m_x; }
    const auto& y()const{ return m_y; }
    const auto& w()const{ return m_w; }
    const auto& h()const{ return m_h; }
    const auto& name()const{ return m_name; }
    const auto& state()const{ return st; }
    virtual bool from_json(const nlohmann::json &j)override{
        m_x = j.at("x");
        m_y = j.at("y");
        m_w = j.at("w");
        m_h = j.at("h");
        m_id = j.at("id");
        m_parent_id = j.at("parent_id");
        m_name = j.at("name");
        return true;
    };
    virtual void to_json(nlohmann::json &j)const override{
        j["x"] = m_x;
        j["y"] = m_y;
        j["w"] = m_w;
        j["h"] = m_h;
        j["id"] = m_id;
        j["parent_id"] = m_parent_id;
        j["name"] = m_name;
    }
public slots:
    void set_x(const long &arg){ m_x = arg; emit x_changed(m_x); }
    void set_y(const long &arg){ m_y = arg; emit y_changed(m_y); }
    void set_w(const long &arg){ m_w = arg; emit w_changed(m_w);  }
    void set_h(const long &arg){ m_h = arg; emit h_changed(m_h);  }
    void set_name(const std::string &arg){ m_name = arg; emit name_changed(m_name);  }
    void set_state(const enum state &arg){ st = arg; emit state_changed(st);  }
signals:
    void x_changed(long arg);
    void y_changed(long arg);
    void w_changed(long arg);
    void h_changed(long arg);
    void name_changed(std::string arg);
    void state_changed(enum state st);
};

class gate_view:public view{
    Q_OBJECT
public:
    enum class direction{
        in,
        out
    };
protected:
    friend class sim_interface;
    friend class sim_ui_glue;
    friend class elem_file_saver;
    enum direction m_dir;
    size_t m_bit_width=1;
    int m_value=0;
public:
    gate_view(){
        this->m_w = 10;
        this->m_h = 10;
    }
    virtual ~gate_view(){}

    virtual bool from_json(const nlohmann::json &j)override = 0;
    virtual void to_json(nlohmann::json &j)const override = 0;

    auto dir()const{ return m_dir; }
    const auto& bit_width()const{ return m_bit_width; }
    const auto& value()const{ return m_value; }
public slots:
    void set_dir(enum direction dir){ m_dir = dir; emit dir_changed(m_dir); }
    void set_bit_width(size_t width){ m_bit_width = width; emit bit_width_changed(m_bit_width); }
    void set_value(int value){ m_value = value; emit value_changed(m_value); }
signals:
    void dir_changed(enum direction dir);
    void value_changed(int value);
    void bit_width_changed(size_t width);
};

class gate_view_in:public gate_view{
public:
    gate_view_in():gate_view(){
        this->m_dir = gate_view::direction::in;
    }
    bool from_json(const nlohmann::json &j)override{
        view::from_json(j);
        if(j.at("token") != token()){
            return false;
        }
        m_value = j.at("value");
        m_bit_width = j.at("bit_width");
        m_dir = (enum direction)j.at("direction");
        return true;
    };
    void to_json(nlohmann::json &j)const override{
        view::to_json(j);
        j["token"] = token();
        j["value"] = m_value;
        j["bit_width"] = m_bit_width;
        j["direction"] = (int)m_dir;
    }
};

class gate_view_out:public gate_view{
    friend class sim_ui_glue;
    friend class elem_file_saver;
    std::vector<std::shared_ptr<gate_view_in>> m_ins;
public:
    gate_view_out():gate_view(){
        this->m_dir = gate_view::direction::out;
    }
    auto& ins(){ return m_ins; }
    auto& ins()const{ return m_ins; }
    virtual bool from_json(const nlohmann::json &j){
        view::from_json(j);
        if(j.at("token") != token()){
            return false;
        }
        m_value = j.at("value");
        m_bit_width = j.at("bit_width");
        m_dir = (enum direction)j.at("direction");
        std::vector<std::pair<size_t, size_t>> placeholders;
        j.at("tied").get_to(placeholders);
        for(auto &p:placeholders){
            auto in = std::make_shared<gate_view_in>();
            in->m_parent_id = p.second;
            in->m_id = p.first;
            this->m_ins.emplace_back(in);
        }
        return true;
    };
    virtual void to_json(nlohmann::json &j)const override{
        view::to_json(j);
        j["token"] = token();
        j["value"] = m_value;
        j["bit_width"] = m_bit_width;
        j["direction"] = (int)m_dir;
        std::vector<std::pair<size_t, size_t>> ids;
        ids.reserve(m_ins.size());
        for(auto &conn:m_ins){
            ids.emplace_back(conn->id(), conn->parent_id());
        }
        j["tied"] = std::move(ids);
    }
};

class elem_view:public view{
public:
    enum class direction{
        dir_up,
        dir_right,
        dir_down,
        dir_left
    };
protected:
    friend class sim_ui_glue;
    friend class sim_interface;
    direction m_dir;
    std::vector<std::shared_ptr<gate_view_in>> ins;
    std::vector<std::shared_ptr<gate_view_out>> outs;
public:
    elem_view(){
        this->m_w = 50;
        this->m_h = 50;
        this->m_dir = direction::dir_right;
    }
    auto dir()const{ return m_dir; }
    void set_dir(direction dir){ this->m_dir = dir; };
    virtual bool from_json(const nlohmann::json &j)override{
        view::from_json(j);
        if(j.at("token") != token()){
            return false;
        }
        ins.clear();
        outs.clear();
        for(auto &j_obj:j.at("ins")){
            auto gt = std::make_shared<gate_view_in>();
            gt->from_json(j_obj);
            ins.emplace_back(gt);
        }
        for(auto &j_obj:j.at("outs")){
            auto gt = std::make_shared<gate_view_out>();
            gt->from_json(j_obj);
            outs.emplace_back(gt);
        }
        return true;
    };
    virtual void to_json(nlohmann::json &j)const override{
        view::to_json(j);
        j["token"] = token();
        std::vector<nlohmann::json> ins,outs;
        nlohmann::json tmp;
        for(auto &gt:this->ins){
            gt->to_json(tmp);
            ins.emplace_back(std::move(tmp));
        }
        for(auto &gt:this->outs){
            gt->to_json(tmp);
            outs.emplace_back(std::move(tmp));
        }
        j["ins"] = std::move(ins);
        j["outs"] = std::move(outs);
    }
};

class elem_view_and:public elem_view{};
class elem_view_nand:public elem_view{};
class elem_view_or:public elem_view{};
class elem_view_nor:public elem_view{};
class elem_view_xor:public elem_view{};
class elem_view_xnor:public elem_view{};
class elem_view_not:public elem_view{};
class elem_view_gate:public elem_view{};
class elem_view_in:public elem_view_gate{
    friend class sim_ui_glue;
    friend class sim_interface;
    std::shared_ptr<gate_view_in> gt_outer;
public:
    virtual bool from_json(const nlohmann::json &j)override{
        elem_view::from_json(j);
        if(j.at("token") != token()){
            return false;
        }
        gt_outer = std::make_shared<gate_view_in>();
        gt_outer->from_json(j.at("gate_outer"));
        return true;
    }
    virtual void to_json(nlohmann::json &j)const override{
        elem_view::to_json(j);
        j["token"] = token();
        nlohmann::json tmp;
        gt_outer->to_json(tmp);
        j["gate_outer"] = tmp;
    }
};
class elem_view_out:public elem_view_gate{
    friend class sim_ui_glue;
    friend class sim_interface;
    std::shared_ptr<gate_view_out> gt_outer;
public:
    bool from_json(const nlohmann::json &j)override{
        elem_view::from_json(j);
        if(j.at("token") != token()){
            return false;
        }
        gt_outer = std::make_shared<gate_view_out>();
        gt_outer->from_json(j.at("gate_outer"));
        return true;
    }
    void to_json(nlohmann::json &j)const override{
        elem_view::to_json(j);
        j["token"] = token();
        nlohmann::json tmp;
        gt_outer->to_json(tmp);
        j["gate_outer"] = tmp;
    }
};
struct elem_view_meta:elem_view{};
