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
    size_t w, h;
public:
	sim_interface(QWidget *parent = nullptr)
        :draw_widget(parent),
        glue(sim_ui_glue::get_instance())
    {
        QWidget::setMouseTracking(true);
        w = 1000;
        h = 1000;
        this->cam.x = 0;
        this->cam.y = 0;
        this->cam.zoom = 1.;
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
    }

    void mousePressEvent(QMouseEvent *e)override{
        auto x = e->x();
        auto y = e->y();

        if(elem && (e->buttons() & Qt::LeftButton)){
            auto w = 10;
            auto h = 10;
            auto id = elem->get_id();
            glue.add_element(x, y, w, h, id);
            sim.add_element(std::move(elem));
            elem = nullptr;
            update();
        }
    }

    void paintEvent(QPaintEvent *e) {
        draw_widget::clean();
        auto x = this->cam.x;
        auto y = this->cam.y;
        auto w = 10;
        auto h = 10;
        auto items = glue.find_elements(x, y, this->width(), this->height());
        qDebug()<<items.size();
        for(auto &el:items){
            draw_widget::draw_rect(el.x-x, el.y-y, w, h);
        }
        if(elem){
            draw_widget::draw_rect(view.x, view.y, w, h);
        }
        this->draw_widget::paintEvent(e);
    }
public slots:
    void add_elem_and(){
        elem = std::make_unique<elem_and>("add");
    }
    void add_elem_or(){
        elem = std::make_unique<elem_or>("or");
    }
    void add_elem_not(){
        elem = std::make_unique<elem_not>("not");
    }
};