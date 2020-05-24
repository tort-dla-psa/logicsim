#pragma once
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPoint>
#include <optional>
#include <vector>
#include "draw_widget.h"
#include "sim/sim.h"
#include "sim/element.h"
#include "k_tree.hpp"
#include "sim_ui_glue.h"

class prop_pair;
class sim_interface : public draw_widget {
	Q_OBJECT

    struct cam{
        long x=0, y=0;
        double zoom=1.;
    }cam;

    std::optional<QPoint> mouse_pos_prev, mouse_pos_prev_move, mouse_pos;
    sim_ui_glue glue;

    std::shared_ptr<elem_view> view;
    std::vector<std::shared_ptr<elem_view>> selected_views;
    std::shared_ptr<gate_view> gate_view_1, gate_view_2;

    class sim sim;

    size_t w=1000, h=1000,
        default_elem_width=50,
        default_elem_height=50,
        default_gate_w=10,
        default_gate_h=10;

    enum class mode{
        still,
        create,
        select,
        select_frame,
        connect_gates
    }mode;

    std::vector<int> pressed_keys;
    QPoint cm_pos;

    void connect_gates(std::shared_ptr<gate_view> gate_view_1, std::shared_ptr<gate_view> gate_view_2);

    std::shared_ptr<elem_view> elem_to_view(const std::unique_ptr<element> &elem);
    std::shared_ptr<elem_view> elem_to_view(size_t id);
    void place_ins(std::shared_ptr<elem_view> &view, const std::unique_ptr<element> &elem);
    void place_outs(std::shared_ptr<elem_view> &view, const std::unique_ptr<element> &elem);

    void draw_elem_view(QPainter &pnt, const std::shared_ptr<elem_view> &view);
    void rotate_view(std::shared_ptr<elem_view> &view);

    void draw_and(QPainter &p, int x, int y, int w, int h);
    void draw_or(QPainter &p, int x, int y, int w, int h);
    void draw_not(QPainter &p, int x, int y, int w, int h);
    void draw_meta(QPainter &p, const std::string &name, int x, int y, int w, int h);
    void draw_out(QPainter &p, int x, int y, int w, int h);
    void draw_in(QPainter &p, int x, int y, int w, int h);
    void try_tick();

    template<class Elem>
    void create_elem(const std::string &name){
        if(this->view){
            this->view->st = elem_view::state::normal;
        }
        this->mode = mode::create;
        auto elem = std::make_unique<Elem>(name);
        auto root_id = this->glue.get_root()->id;
        auto root_it = sim.get_by_id(root_id);
        auto &ref = *sim.emplace(root_it, std::move(elem));
        this->view = elem_to_view(ref);
        this->view->st = elem_view::state::creating;
    }

    void set_in_value(std::shared_ptr<elem_view_in> view);
    void dive_into_meta(std::shared_ptr<elem_view_meta> view);

    void show_menu_for_elements(const QPoint &p);
    void show_menu_for_element(const QPoint &p);
    void show_menu_for_none(const QPoint &p);
public:
	sim_interface(QWidget *parent = nullptr);
    virtual ~sim_interface();

    void mouseMoveEvent(QMouseEvent *e)override;
    void mousePressEvent(QMouseEvent *e)override;
    void mouseReleaseEvent(QMouseEvent *e)override;
    void keyPressEvent(QKeyEvent* e)override;
    void keyReleaseEvent(QKeyEvent* e)override;
    void paintEvent(QPaintEvent *e)override;
public slots:
    void add_elem_and();
    void add_elem_nand();
    void add_elem_or();
    void add_elem_xor();
    void add_elem_nor();
    void add_elem_xnor();
    void add_elem_not();
    void add_elem_in();
    void add_elem_out();
    void add_elem_meta();
    void save_sim(QString path);
    void load_sim(QString path);

    void slot_propery_changed(const prop_pair* prop);
    void showContextMenu(const QPoint &p);
private slots:
    void delete_item_cm();
    void cut_item_cm();
    void delete_items_cm();
    void cut_items_cm();
    void go_up_cm();
signals:
    void element_selected(std::shared_ptr<elem_view> view);
    void element_changed(std::shared_ptr<elem_view> view);
};