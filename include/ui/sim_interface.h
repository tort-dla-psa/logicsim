#pragma once
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <optional>
#include <vector>
#include "draw_widget.h"
#include "sim/sim.h"
#include "sim/element.h"
#include "sim_ui_glue.h"

class prop;
class sim_interface : public draw_widget {
	Q_OBJECT

    struct cam{
        long x=0, y=0;
        double zoom=1.;
    }cam;

    std::optional<QPoint> mouse_pos_prev, mouse_pos;
    sim_ui_glue& glue;

    std::optional<size_t> current_id;
    std::shared_ptr<elem_view> view;
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
        connect_gates
    }mode;

    std::vector<int> pressed_keys;

    std::shared_ptr<elem_view> elem_to_view(const std::shared_ptr<element> &elem);
    std::shared_ptr<elem_view> elem_to_view(size_t id);
    void draw_elem_view(QPainter &pnt, const std::shared_ptr<elem_view> &view);
    void rotate_view(std::shared_ptr<elem_view> &view);

    size_t vec_to_val(const std::vector<bool> &vec);
    std::vector<bool> val_to_vec(const size_t &vec);
public:
	sim_interface(QWidget *parent = nullptr);
    virtual ~sim_interface();

    void mouseMoveEvent(QMouseEvent *e)override;
    void mousePressEvent(QMouseEvent *e)override;
    void mouseReleaseEvent(QMouseEvent *e)override;
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void paintEvent(QPaintEvent *e);
public slots:
    void add_elem_and();
    void add_elem_or();
    void add_elem_not();
    void add_elem_in();
    void add_elem_out();

    void slot_propery_changed(const class prop* prop);
signals:
    void element_selected(std::shared_ptr<elem_view> view);
    void element_changed(std::shared_ptr<elem_view> view);
};