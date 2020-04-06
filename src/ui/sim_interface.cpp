#include "sim_interface.h"
#include <QInputDialog>

#define DEBUG_VIEW
#define DEBUG_SIM_GLUE

std::shared_ptr<elem_view> sim_interface::elem_to_view(const std::unique_ptr<element> &elem){
    auto view = std::make_shared<elem_view>();
    auto id = elem->get_id();

    view->w = this->default_elem_width;
    view->h = this->default_elem_height;
    view->id = id;
    view->dir = elem_view::direction::dir_right;

    view->t = dynamic_cast<elem_and*>(elem.get())? elem_view::type::type_and:
        dynamic_cast<elem_or*>(elem.get())? elem_view::type::type_or:
        dynamic_cast<elem_not*>(elem.get())? elem_view::type::type_not:
        dynamic_cast<elem_in*>(elem.get())? elem_view::type::type_in:
        dynamic_cast<elem_out*>(elem.get())? elem_view::type::type_out:
        elem_view::type::type_custom;

    if(elem->get_ins() != 0){
        long ins_offset = view->h/elem->get_ins();
        long ins_x = 0;
        long ins_y = ins_offset/2;
        for(size_t i=0; i<elem->get_ins(); i++){
            auto gt = std::make_shared<gate_view_in>();
            gt->w = this->default_gate_w;
            gt->h = this->default_gate_h;
            gt->x = ins_x;
            gt->y = ins_y;
            if(view->t == elem_view::type::type_in){
                gt->id = ((elem_in*)elem.get())->get_in_id();
            }else{
                gt->id = elem->get_in(i)->get_id();
            }
            gt->parent = view;
            ins_y += ins_offset;
            view->gates_in.emplace_back(gt);
        }
    }
    if(elem->get_outs() != 0){
        long outs_offset = view->h/elem->get_outs();
        long outs_x = view->w;
        long outs_y = outs_offset/2;
        for(size_t i=0; i<elem->get_outs(); i++){
            auto gt = std::make_shared<gate_view_out>();
            gt->w = this->default_gate_w;
            gt->h = this->default_gate_h;
            gt->y = outs_y;
            gt->x = outs_x;
            if(view->t == elem_view::type::type_out){
                gt->id = ((elem_out*)elem.get())->get_out_id();
            }else{
                gt->id = elem->get_out(i)->get_id();
            }
            gt->parent = view;
            outs_y += outs_offset;
            view->gates_out.emplace_back(gt);
        }
    }
    return view;
}

void sim_interface::rotate_view(std::shared_ptr<elem_view> &view){
    auto rotate = [&view](long &x, long &y){
        QPoint coords(x, y);
        QTransform t;
        if(view->dir == elem_view::direction::dir_left){
            t.rotate(180);
            t.translate(-view->w, -view->h);
        }else if(view->dir == elem_view::direction::dir_up){
            t.rotate(90);
            t.translate(0, -view->h);
        }else if(view->dir == elem_view::direction::dir_down){
            t.rotate(-90);
            t.translate(-view->w, 0);
        }
        coords = t.map(coords);
        x = coords.x();
        y = coords.y();
    };
    for(auto &gate_in:view->gates_in){
        rotate(gate_in->x, gate_in->y);
    }
    for(auto &gate_out:view->gates_out){
        rotate(gate_out->x, gate_out->y);
    }
}

void sim_interface::draw_elem_view(QPainter &pnt, const std::shared_ptr<elem_view> &view){
    long draw_x = (view->x-cam.x)*cam.zoom;
    long draw_y = (view->y-cam.y)*cam.zoom;
    long draw_w = (view->w)*cam.zoom;
    long draw_h = (view->h)*cam.zoom;
    QImage img(draw_w+this->default_gate_w*cam.zoom+1,
        draw_h+this->default_gate_h*cam.zoom+1,
        QImage::Format_RGB32);
    img.fill(Qt::white);
    pnt.begin(&img);
    pnt.drawRect(0+this->default_gate_w/2,
        0+this->default_gate_h/2, draw_w, draw_h);

    auto &gates_in = view->gates_in;
    for(auto &gate:gates_in){
        pnt.drawRect(gate->x*cam.zoom, gate->y*cam.zoom,
            gate->w*cam.zoom, gate->h*cam.zoom);
    }

    auto &gates_out = view->gates_out;
    for(auto &gate:gates_out){
        pnt.drawRect(gate->x*cam.zoom, gate->y*cam.zoom,
            gate->w*cam.zoom, gate->h*cam.zoom);
    }

    if(view->t == elem_view::type::type_in){
        auto bit_val = sim.get_in_value(view->id, 0);
        auto val = vec_to_val(bit_val);
        pnt.drawText(view->x, view->y, QString::number(val));
    }else if(view->t == elem_view::type::type_out){
        auto bit_val = sim.get_out_value(view->id, 0);
        auto val = vec_to_val(bit_val);
        pnt.drawText(view->x, view->y, QString::number(val));
    }
    pnt.end();

    draw_widget::draw_image(draw_x, draw_y, img);
    for(auto &gate:gates_out){
        for(auto &gt:gate->gates_in){
            draw_widget::draw_line(gate->x+gate->parent->x,
                gate->y+gate->parent->y,
                gt->x+gt->parent->x,
                gt->y+gt->parent->y);
        }
    }

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
}

size_t sim_interface::vec_to_val(const std::vector<bool> &vec){
    size_t result = 0;
    for(size_t i=0; i<sizeof(size_t)*8; i++){
        result |= vec.at(i);
        result <<= 1;
    }
    return result;
}
std::vector<bool> sim_interface::val_to_vec(const size_t &val){
    std::vector<bool> result;
    for(size_t i=0; i<sizeof(size_t)*8; i++){
        bool tmp = (val >> (sizeof(size_t)*8-1))&1;
        result.push_back(tmp);
    }
    return result;
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
    if(mode == mode::select && view){
        if(e->buttons() & Qt::LeftButton){
            view->x = x;
            view->y = y;
            glue.add_view(view);
            update();
            return;
        }
    }
    if(this->mouse_pos_prev.has_value()){
        this->mouse_pos = e->pos();
        update();
        return;
    }
}

void sim_interface::mousePressEvent(QMouseEvent *e){
    this->mouse_pos_prev = e->pos();
    auto x = e->x();
    auto y = e->y();

    if(elem){
        if(e->buttons() & Qt::LeftButton){
            glue.add_view(view);
            sim.add_element(std::move(elem));
            elem = nullptr;
            view = nullptr;
            update();
        }
    }else{
        if(e->buttons() & Qt::LeftButton){
            auto items = glue.find_views(x, y);
            if(!items.empty()){
                this->view = items.front();
                if(this->view->t == elem_view::type::type_out){
                    int input = QInputDialog::getInt(this, "input value", "input value to pass");
                    std::vector<bool> bits(sizeof(input)*8);
                    int most_right_bit=0;
                    for(size_t i=0; i<bits.size(); i++){
                        bool bit = (input>>i)&1;
                        if(bit){
                            most_right_bit = i;
                        }
                        bits.at(i) = bit;
                    }
                    auto out_width = sim.get_out_width(view->id);
                    if(most_right_bit > out_width){
                        //TODO:warning
                        bits.erase(bits.begin()+most_right_bit, bits.end());
                    }else if(most_right_bit < out_width){
                        bits.resize(out_width);
                    }
                    sim.set_out_value(this->view->id, bits);
                }else{
                    this->mode = mode::select;
                }
            }
            auto gates = glue.get_gates(x, y);
            if(!gates.empty()){
                this->mode = mode::connect_gates;
                this->gate_view_1 = gates.front();
            }
        }
    }
#ifdef DEBUG_SIM_GLUE
    {
        qDebug()<<"views:";
        const auto &all_views = glue.access_views();
        for(const auto &view:all_views){
            qDebug()<<"view id:"<<view->id
                <<"\tx:"<<view->x <<"\ty:"<<view->y
                <<"\tw:"<<view->w <<"\th:"<<view->h;
            for(const auto &gt:view->gates_in){
                qDebug()<<"\tgt_in id:"<<gt->id
                    <<"\tx:"<<gt->x <<"\ty:"<<gt->y
                    <<"\tw:"<<gt->w <<"\th:"<<gt->h;
            }
            for(const auto &gt:view->gates_out){
                qDebug()<<"\tgt_out id:"<<gt->id
                    <<"\tx:"<<gt->x <<"\ty:"<<gt->y
                    <<"\tw:"<<gt->w <<"\th:"<<gt->h;
            }
        }
        qDebug()<<"";
    }
#endif
}

void sim_interface::mouseReleaseEvent(QMouseEvent *e){
    auto x = e->x();
    auto y = e->y();
    this->mouse_pos.reset();
    this->mouse_pos_prev.reset();
    if(this->mode == mode::select && view){
        view = nullptr;
    }
    if(this->mode == mode::connect_gates && this->gate_view_1) {
        auto gates = glue.get_gates(x, y);
        if(!gates.empty()){
            this->mode = mode::connect_gates;
            this->gate_view_2 = gates.front();
            sim.connect_gates(gate_view_1->id, gate_view_2->id);
            auto cast_in = std::dynamic_pointer_cast<gate_view_in>(this->gate_view_1);
            auto cast_out = std::dynamic_pointer_cast<gate_view_out>(this->gate_view_1);
            if(cast_in){
                auto cast_out_2 = std::dynamic_pointer_cast<gate_view_out>(this->gate_view_2);
                if(cast_out_2){
                    cast_out_2->gates_in.emplace_back(cast_in);
                }else{ //TODO:throw dialog
                }
            }else if(cast_out){
                auto cast_in_2 = std::dynamic_pointer_cast<gate_view_in>(this->gate_view_2);
                if(cast_in_2){
                    cast_out->gates_in.emplace_back(cast_in_2);
                }else{ //TODO:throw dialog
                }
            }
        }
        this->gate_view_1 = this->gate_view_2 = nullptr;
    }
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
            rotate_view(view);
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
        if(view && (el->id == view->id)){
            continue;
        }
        draw_elem_view(pnt, el);
    }
    //to draw created or selected element
    if(mode == mode::create && elem){
        if(view){
            draw_elem_view(pnt, view);
        }else{
            qWarning()<<"no view for element";
        }
    }else if(mode == mode::select){
        if(view){
            draw_elem_view(pnt, view);
        }
    }
    if(mouse_pos_prev.has_value() && mouse_pos.has_value()){
        if(mode == mode::connect_gates){
            draw_line(mouse_pos->x(), mouse_pos->y(),
                mouse_pos_prev->x(), mouse_pos_prev->y());
        }else{
            draw_rect(std::min(mouse_pos->x(), mouse_pos_prev->x()),
                std::min(mouse_pos->y(), mouse_pos_prev->y()),
                std::abs(mouse_pos->x() - mouse_pos_prev->x()),
                std::abs(mouse_pos->y() - mouse_pos_prev->y()));
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
void sim_interface::add_elem_in(){
    this->mode = mode::create;
    elem = std::make_unique<elem_in>("in");
    this->view = elem_to_view(elem);
}
void sim_interface::add_elem_out(){
    this->mode = mode::create;
    elem = std::make_unique<elem_out>("out");
    this->view = elem_to_view(elem);
}