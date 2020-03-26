#include "sim_interface.h"

#define DEBUG_VIEW
#define DEBUG_SIM_GLUE

struct elem_view sim_interface::elem_to_view(const std::unique_ptr<element> &elem){
    struct elem_view view;
    auto id = elem->get_id();

    view.w = this->default_elem_width;
    view.h = this->default_elem_height;
    view.id = id;
    view.dir = elem_view::direction::dir_right;

    view.t = ((elem_and*)elem.get())? elem_view::type::type_and:
        ((elem_or*)elem.get())? elem_view::type::type_or:
        ((elem_not*)elem.get())? elem_view::type::type_not:
        elem_view::type::type_custom;
    view.gates_in.resize(elem->get_ins());
    view.gates_out.resize(elem->get_outs());
    rotate_view(view);
    return view;
}

void sim_interface::rotate_view(elem_view &view){
    std::vector<gate_view> gates_in, gates_out;
    long ins_offset, outs_offset, ins_x, ins_y, outs_x, outs_y;
    ins_offset = view.h/view.gates_in.size();
    outs_offset = view.h/view.gates_out.size();
    ins_x = 0;
    ins_y = ins_offset/2;
    outs_x = view.w;
    outs_y = outs_offset/2;
    
    auto rotate = [&view](long &x, long &y){
        QPoint coords(x, y);
        QTransform t;
        if(view.dir == elem_view::direction::dir_left){
            t.rotate(180);
            t.translate(-view.w, -view.h);
        }else if(view.dir == elem_view::direction::dir_up){
            t.rotate(90);
            t.translate(0, -view.h);
        }else if(view.dir == elem_view::direction::dir_down){
            t.rotate(-90);
            t.translate(-view.w, 0);
        }
        coords = t.map(coords);
        x = coords.x();
        y = coords.y();
    };
    for(size_t i = 0; i < view.gates_in.size(); i++){
        gate_view gt(gate_view::direction::in);
        gt.w = this->default_gate_w;
        gt.h = this->default_gate_h;
        gt.x = ins_x;
        gt.y = ins_y;
        rotate(gt.x, gt.y);
        ins_y += ins_offset;
        gates_in.emplace_back(gt);
    }
    for(size_t i = 0; i < view.gates_out.size(); i++){
        gate_view gt(gate_view::direction::out);
        gt.w = this->default_gate_w;
        gt.h = this->default_gate_h;
        gt.y = outs_y;
        gt.x = outs_x;
        rotate(gt.x, gt.y);
        outs_y += outs_offset;
        gates_out.emplace_back(gt);
    }
    view.gates_in = gates_in;
    view.gates_out = gates_out;
}

void sim_interface::draw_elem_view(QPainter &pnt, const struct elem_view &view){
    long draw_x = (view.x-cam.x)*cam.zoom;
    long draw_y = (view.y-cam.y)*cam.zoom;
    long draw_w = (view.w)*cam.zoom;
    long draw_h = (view.h)*cam.zoom;
    QImage img(draw_w+this->default_gate_w*cam.zoom+1,
        draw_h+this->default_gate_h*cam.zoom+1,
        QImage::Format_RGB32);
    img.fill(Qt::white);
    pnt.begin(&img);
    pnt.drawRect(0+this->default_gate_w/2,
        0+this->default_gate_h/2, draw_w, draw_h);

    auto &gates_in = view.gates_in;
    for(auto &gate:gates_in){
        pnt.drawRect(gate.x*cam.zoom, gate.y*cam.zoom,
            gate.w*cam.zoom, gate.h*cam.zoom);
    }

    auto &gates_out = view.gates_out;
    for(auto &gate:gates_out){
        pnt.drawRect(gate.x*cam.zoom, gate.y*cam.zoom,
            gate.w*cam.zoom, gate.h*cam.zoom);
    }
    pnt.end();

    draw_widget::draw_image(draw_x, draw_y, img);

#ifdef DEBUG_GATES
    for(auto &gate:gates_out){
        QString txt = QString::number(gate.x)+" "+QString::number(gate.y);
        draw_text(draw_x+gate.x, draw_y+gate.y, txt);
    }
    for(auto &gate:gates_in){
        QString txt = QString::number(gate.x)+" "+QString::number(gate.y);
        draw_text(draw_x+gate.x, draw_y+gate.y, txt);
    }
#endif
#ifdef DEBUG_VIEW
    {
        QString txt = QString::number(view.x)+" "+QString::number(view.y);
        draw_text(draw_x, draw_y, txt);
    }
#endif
}

sim_interface::sim_interface(QWidget* parent)
    :draw_widget(parent),
    glue(sim_ui_glue::get_instance())
{
    setFocus();
    QWidget::setMouseTracking(true);
}
sim_interface::~sim_interface(){}

void sim_interface::mouseMoveEvent(QMouseEvent *e){
    setFocus();
    auto x = e->x();
    auto y = e->y();

    if(mode == mode::create && elem){
        view->x = x;
        view->y = y;
        update();
        return;
    }
    if(mode == mode::select && view.has_value()){
        if(e->buttons() & Qt::LeftButton){
            view->x = x;
            view->y = y;
            glue.add_view(*view);
            update();
            return;
        }
    }
}

void sim_interface::mousePressEvent(QMouseEvent *e){
    auto x = e->x();
    auto y = e->y();

    if(elem){
        if(e->buttons() & Qt::LeftButton){
            glue.add_view(view.value());
            sim.add_element(std::move(elem));
            elem = nullptr;
            view.reset();
            update();
        }
    }else{
        if(e->buttons() & Qt::LeftButton){
            auto items = glue.find_views(x, y);
            if(!items.empty()){
                this->view.emplace(items.front());
                this->mode = mode::select;
            }
        }
    }
#ifdef DEBUG_SIM_GLUE
    {
        qDebug()<<"views:";
        const auto &all_views = glue.access_views();
        for(const auto &view:all_views){
            qDebug()<<"view id:"<<view.id
                <<"\tx:"<<view.x <<"\ty:"<<view.y
                <<"\tw:"<<view.w <<"\th:"<<view.h;
        }
        qDebug()<<"";
    }
#endif
}

void sim_interface::keyPressEvent(QKeyEvent* e){
    pressed_keys.emplace_back(e->key());
    bool ctrl = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Control) != pressed_keys.end();
    bool left = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Left) != pressed_keys.end();
    bool right = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Right) != pressed_keys.end();
    bool up = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Up) != pressed_keys.end();
    bool down = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Down) != pressed_keys.end();
    bool plus = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Equal) != pressed_keys.end();
    bool minus = std::find(pressed_keys.begin(), pressed_keys.end(),
        Qt::Key_Minus) != pressed_keys.end();
    if(elem){
        if(ctrl && (left || right)){
            int cast = (int)view->dir;
            if(left){
                cast ++;
            }else{
                cast --;
                cast += 4;
            }
            cast %= 4;
            view->dir = (elem_view::direction)cast;
            rotate_view(view.value());
            return;
        }
    }
    if(plus){
        cam.zoom += 0.25;
    }else if(minus){
        cam.zoom -= 0.25;
    }
    if(plus || minus){
        return;
    }
    if(ctrl){
        if(left){
            cam.x -= this->default_elem_width*2;
        }else if(right){
            cam.x += this->default_elem_width*2;
        }else if(up){
            cam.y -= this->default_elem_height*2;
        }else if(down){
            cam.y += this->default_elem_height*2;
        }
        if(left || right || up || down){
            return;
        }
    }
}

void sim_interface::keyReleaseEvent(QKeyEvent* e){
    auto it = std::find(pressed_keys.begin(), pressed_keys.end(),
        e->key());
    if(it != pressed_keys.end()){
        pressed_keys.erase(it);
    }
}

void sim_interface::paintEvent(QPaintEvent *e) {
    draw_widget::clean();
    auto x = this->cam.x;
    auto y = this->cam.y;
    auto items = glue.find_views(x, y,
        this->width()/cam.zoom, this->height()/cam.zoom);

    QPainter pnt;
    for(auto &el:items){
        //to skip selected item view
        if(view.has_value() && (el.id == view->id)){
            continue;
        }
        draw_elem_view(pnt, el);
    }
    //to draw created or selected element
    if(mode == mode::create && elem){
        if(view.has_value()){
            draw_elem_view(pnt, view.value());
        }else{
            qWarning()<<"no view for element";
        }
    }else if(mode == mode::select){
        if(view.has_value()){
            draw_elem_view(pnt, view.value());
        }
    }
    this->draw_widget::paintEvent(e);
}

void sim_interface::add_elem_and(){
    this->mode = mode::create;
    elem = std::make_unique<elem_and>("add");
    this->view = elem_to_view(elem);
}
void sim_interface::add_elem_or(){
    this->mode = mode::create;
    elem = std::make_unique<elem_or>("or");
    this->view = elem_to_view(elem);
}
void sim_interface::add_elem_not(){
    this->mode = mode::create;
    elem = std::make_unique<elem_not>("not");
    this->view = elem_to_view(elem);
}