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

class sim_interface : public draw_widget {
	Q_OBJECT

    struct cam{
        long x=0, y=0;
        double zoom=1.;
    }cam;

    std::optional<QPoint> mouse_pos_prev, mouse_pos;
    sim_ui_glue& glue;
    std::unique_ptr<element> elem;
    std::optional<elem_view> view;
	class sim sim;
    size_t w=1000, h=1000,
        default_elem_width=50,
        default_elem_height=50,
        default_gate_w=10,
        default_gate_h=10;

    enum class mode{
        create,
        select,
    }mode;

    std::vector<int> pressed_keys;

    struct elem_view elem_to_view(const std::unique_ptr<element> &elem);
    void draw_elem_view(QPainter &pnt, const struct elem_view &view);
    void rotate_view(struct elem_view &view);
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
};