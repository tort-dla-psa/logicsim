#pragma once
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <vector>
#include "draw_widget.h"
#include "sim/sim.h"
#include "sim/element.h"
#include "sim_ui_glue.h"

class sim_interface : public draw_widget {
	Q_OBJECT

    struct cam{
        long x, y;
        double zoom;
    }cam;

    std::unique_ptr<element> elem;
    sim_ui_glue& glue;
    elem_view view;
	class sim sim;
    size_t w, h,
        default_elem_width,
        default_elem_height,
        default_gate_w,
        default_gate_h;

    std::vector<int> pressed_keys;

    struct elem_view elem_to_view(const std::unique_ptr<element> &elem){
        struct elem_view view;
        auto id = elem->get_id();

        view.w = this->default_elem_width;
        view.h = this->default_elem_height;
        view.id = id;

        view.t = ((elem_and*)elem.get())? elem_view::type::type_and:
            ((elem_or*)elem.get())? elem_view::type::type_or:
            ((elem_not*)elem.get())? elem_view::type::type_not:
            elem_view::type::type_custom;

        for(size_t i = 0; i < elem->get_ins(); i++){
            view.gates_in.emplace_back(
                gate_view::direction::in
            );
        }
        for(size_t i = 0; i < elem->get_outs(); i++){
            view.gates_out.emplace_back(
                gate_view::direction::out
            );
        }
        return view;
    }
    void draw_elem_view(QPainter &pnt, const struct elem_view &view){
        long draw_x = (view.x-cam.x)*cam.zoom;
        long draw_y = (view.y-cam.y)*cam.zoom;
        long draw_w = (view.w)*cam.zoom;
        long draw_h = (view.h)*cam.zoom;
        QImage img(draw_w+this->default_gate_w,
            draw_h+this->default_gate_h, QImage::Format_RGB32);
        img.fill(Qt::white);
        pnt.begin(&img);
        pnt.drawRect(0+this->default_gate_w/2,
            0+this->default_gate_h/2, draw_w, draw_h);

        auto &gates_in = view.gates_in;
        long ins_y = 0;
        long ins_offset = draw_w/(gates_in.size());
        long ins_x = ins_offset/2;
        for(auto &gate:gates_in){
            pnt.drawRect(ins_x, ins_y,
                this->default_gate_w,
                this->default_gate_h);
            ins_x+=ins_offset;
        }

        auto &gates_out = view.gates_out;
        long outs_y = draw_h-1;
        long outs_offset = draw_w/(gates_out.size());
        long outs_x = outs_offset/2;
        for(auto &gate:gates_out){
            pnt.drawRect(outs_x, outs_y,
                this->default_gate_w,
                this->default_gate_h);
            outs_y+=outs_offset;
        }
        pnt.end();

        QPoint center = img.rect().center();
        QMatrix matrix;
        matrix.translate(center.x(), center.y());
        if(view.dir == elem_view::direction::dir_down){
            matrix.rotate(0);
        }else if(view.dir == elem_view::direction::dir_right){
            matrix.rotate(-90);
        }else if(view.dir == elem_view::direction::dir_up){
            matrix.rotate(180);
        }else if(view.dir == elem_view::direction::dir_left){
            matrix.rotate(90);
        }
        QImage img_dst = img.transformed(matrix);
        draw_widget::draw_image(draw_x, draw_y, img_dst);
    }
public:
	sim_interface(QWidget *parent = nullptr)
        :draw_widget(parent),
        glue(sim_ui_glue::get_instance())
    {
        setFocus();
        QWidget::setMouseTracking(true);
        w = 1000;
        h = 1000;
        this->cam.x = 0;
        this->cam.y = 0;
        this->cam.zoom = 1.;
        this->default_elem_height = 50;
        this->default_elem_width = 50;
        this->default_gate_w = 10;
        this->default_gate_h = 10;
    }
    virtual ~sim_interface(){}

    void mouseMoveEvent(QMouseEvent *e)override{
        auto x = e->x();
        auto y = e->y();

        if(elem){
            view.x = x;
            view.y = y;
            update();
        }
        setFocus();
    }

    void wheelEvent(QWheelEvent* e)override{
        //cam.zoom += e->pixelDelta().x();
    }

    void mousePressEvent(QMouseEvent *e)override{
        auto x = e->x();
        auto y = e->y();

        if(elem && (e->buttons() & Qt::LeftButton)){
            glue.add_element(view);
            sim.add_element(std::move(elem));
            elem = nullptr;
            view = elem_view();
            update();
        }
    }

    void keyPressEvent(QKeyEvent* e){
        pressed_keys.emplace_back(e->key());
        if(elem){
            bool ctrl = std::find(pressed_keys.begin(), pressed_keys.end(),
                Qt::Key_Control)
                != pressed_keys.end();
            if(!ctrl){
                return;
            }
            bool left = std::find(pressed_keys.begin(), pressed_keys.end(),
                Qt::Key_Left)
                != pressed_keys.end();
            if(left){
                int cast = (int)view.dir;
                cast ++;
                cast %= 4;
                view.dir = (elem_view::direction)cast;
                return;
            }
            bool right = std::find(pressed_keys.begin(), pressed_keys.end(),
                Qt::Key_Right)
                != pressed_keys.end();
            if(right){
                int cast = (int)view.dir;
                cast --;
                cast += 4;
                cast %= 4;
                view.dir = (elem_view::direction)cast;
                return;
            }
        }
    }
    void keyReleaseEvent(QKeyEvent* e){
        auto it = std::find(pressed_keys.begin(), pressed_keys.end(),
            e->key());
        if(it != pressed_keys.end()){
            pressed_keys.erase(it);
        }
    }

    void paintEvent(QPaintEvent *e) {
        draw_widget::clean();
        auto x = this->cam.x;
        auto y = this->cam.y;
        auto items = glue.find_elements(x, y,
            this->width(), this->height());

        QPainter pnt;
        for(auto &el:items){
            draw_elem_view(pnt, el);
        }
        if(elem){
            draw_elem_view(pnt, view);
        }
        this->draw_widget::paintEvent(e);
    }
public slots:
    void add_elem_and(){
        elem = std::make_unique<elem_and>("add");
        this->view = elem_to_view(elem);
    }
    void add_elem_or(){
        elem = std::make_unique<elem_or>("or");
        this->view = elem_to_view(elem);
    }
    void add_elem_not(){
        elem = std::make_unique<elem_not>("not");
        this->view = elem_to_view(elem);
    }
};