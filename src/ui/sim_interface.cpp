#include "sim_interface.h"
#include "properties.h"
#include "prop_pair.h"
#include "sim/bit_math.h"
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QPen>

elem_meta::elem_vec::const_iterator sim_interface::find_by_id(const size_t &id)const{
    return std::find_if(sim_root->get_begin(), sim_root->get_end(),
    [&id](auto &el){
        return el->get_id() == id;
    });
}

void sim_interface::connect_gates(std::shared_ptr<gate_view> gate_view_1, std::shared_ptr<gate_view> gate_view_2){
    bool valid = false;
    auto gt_1 = sim_root->find_gate(gate_view_1->id);
    auto gt_2 = sim_root->find_gate(gate_view_2->id);
    auto func = [](auto gt_1, auto gt_2){
        auto in_cast = std::dynamic_pointer_cast<gate_in>(gt_1);
        auto out_cast = std::dynamic_pointer_cast<gate_out>(gt_2);
        if(in_cast && out_cast){
            out_cast->tie_input(in_cast);
            return true;
        }
        return false;
    };

    valid |= func(gt_1, gt_2);
    if(!valid){
        valid |= func(gt_2, gt_1);
    }
    if(!valid){
        qWarning("wrong connection");
    }
    glue.tie_gates(gate_view_1, gate_view_2, valid);
    return;
}

std::shared_ptr<elem_view> sim_interface::elem_to_view(size_t id){
    const auto &el = sim_root->find_element(id);
    return elem_to_view(el);
}
std::shared_ptr<elem_view> sim_interface::elem_to_view(const std::unique_ptr<element> &elem){
    std::shared_ptr<elem_view> view;
    std::shared_ptr<elem_view_in> in_cast;
    std::shared_ptr<elem_view_out> out_cast;
    elem_in* elem_in_cast;
    elem_out* elem_out_cast;
    if(elem_in_cast = dynamic_cast<class elem_in*>(elem.get())){
        in_cast = std::make_shared<elem_view_in>();
        view = in_cast;
    }else if(elem_out_cast = dynamic_cast<class elem_out*>(elem.get())){
        out_cast = std::make_shared<elem_view_out>();
        view = out_cast;
    }else if(dynamic_cast<class elem_and*>(elem.get())){
        view = std::make_shared<elem_view_and>();
    }else if(dynamic_cast<class elem_or*>(elem.get())){
        view = std::make_shared<elem_view_or>();
    }else if(dynamic_cast<class elem_not*>(elem.get())){
        view = std::make_shared<elem_view_not>();
    }else if(dynamic_cast<class elem_meta*>(elem.get())){
        view = std::make_shared<elem_view_meta>();
    }
    auto id = elem->get_id();

    view->name = elem->get_name();
    view->w = this->default_elem_width;
    view->h = this->default_elem_height;
    view->id = id;
    view->dir = elem_view::direction::dir_right;
    view->st = elem_view::state::normal;
    view->parent = glue.get_root();

    place_gates_in(view, elem);
    place_gates_out(view, elem);

    if(elem_in_cast){
        auto outer = std::make_shared<gate_view_in>();
        outer->id = elem_in_cast->get_outer_id();
        in_cast->gt_outer = outer;
    }else if(elem_out_cast){
        auto outer = std::make_shared<gate_view_out>();
        outer->id = elem_out_cast->get_outer_id();
        out_cast->gt_outer = outer;
    }
    return view;
}
void sim_interface::place_gates_in(std::shared_ptr<elem_view> &view,
    const std::unique_ptr<element> &elem)
{
    if(elem->get_ins_size() == 0){
        return;
    }
    long ins_offset = view->h/elem->get_ins_size();
    long ins_x = 0;
    long ins_y = ins_offset/2;
    auto gt_ = std::make_shared<gate_view_in>();
    gt_->w = this->default_gate_w;
    gt_->h = this->default_gate_h;
    gt_->x = ins_x;
    gt_->parent = view;
    for(size_t i=0; i<elem->get_ins_size(); i++){
        auto gt = std::make_shared<gate_view_in>(*gt_);
        gt->y = ins_y;
        ins_y += ins_offset;
        auto gt_in = elem->get_in(i);
        gt->id = gt_in->get_id();
        gt->name = gt_in->get_name();
        gt->bit_width = gt_in->get_width();
        view->gates_in.emplace_back(gt);
    }
}
void sim_interface::place_gates_out(std::shared_ptr<elem_view> &view,
    const std::unique_ptr<element> &elem)
{
    if(elem->get_outs_size() == 0){
        return;
    }
    long outs_offset = view->h/elem->get_outs_size();
    long outs_x = view->w;
    long outs_y = outs_offset/2;
    auto gt_ = std::make_shared<gate_view_out>();
    gt_->w = this->default_gate_w;
    gt_->h = this->default_gate_h;
    gt_->x = outs_x;
    gt_->parent = view;
    for(size_t i=0; i<elem->get_outs_size(); i++){
        auto gt = std::make_shared<gate_view_out>(*gt_);
        gt->y = outs_y;
        outs_y += outs_offset;
        auto gt_out = elem->get_out(i); 
        gt->id = gt_out->get_id();
        gt->name = gt_out->get_name();
        gt->bit_width = gt_out->get_width();
        view->gates_out.emplace_back(gt);
    }
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

void sim_interface::draw_and(QPainter &p, int x, int y, int w, int h){
    QRectF rect(x, y, w, h); 
    QPainterPath path;
    path.moveTo(x, y);
    path.lineTo(x, y+h);
    path.lineTo(x+w/2, y+h);
    path.arcMoveTo(rect, -90);
    path.arcTo(rect, -90, 180);
    path.lineTo(x, y);
    p.drawPath(path);
}
void sim_interface::draw_or(QPainter &p, int x, int y, int w, int h){
    QPainterPath path;
    path.moveTo(x, y);
    path.quadTo(x+w/2, y+h/2, x, y+h);
    path.quadTo(x+w/4*3, y+h, x+w, y+h/2);
    path.quadTo(x+w/4*3, y, x, y);
    p.drawPath(path);
}
void sim_interface::draw_not(QPainter &p, int x, int y, int w, int h){
    QRectF rect(x, y, w, h); 
    QPainterPath path;
    path.moveTo(x, y);
    path.lineTo(x, y+h);
    path.lineTo(x+w, y+h/2);
    path.lineTo(x, y);
    p.drawPath(path);
}
void sim_interface::draw_meta(QPainter &p, int x, int y, int w, int h){
    QRectF rect(x, y, w, h); 
    p.drawRect(rect);
}
void sim_interface::draw_out(QPainter &p, int x, int y, int w, int h){
    QRectF rect(x, y, w, h); 
}
void sim_interface::draw_in(QPainter &p, int x, int y, int w, int h){
    QRectF rect(x, y, w, h); 
}

void sim_interface::draw_elem_view(QPainter &pnt, const std::shared_ptr<elem_view> &view){
    long draw_x = (view->x-cam.x)*cam.zoom;
    long draw_y = (view->y-cam.y)*cam.zoom;
    long draw_w = (view->w)*cam.zoom;
    long draw_h = (view->h)*cam.zoom;
    QImage img(draw_w+this->default_gate_w*cam.zoom+1,
        draw_h+this->default_gate_h*cam.zoom+1,
        QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    pnt.begin(&img);

    if(view->st == elem_view::state::creating){
        pnt.setOpacity(0.6);
    }else if(view->st == elem_view::state::selected){
        pnt.setPen(Qt::DashLine);
        pnt.drawRect(1,1,img.width()-2, img.height()-2);
        pnt.setPen(Qt::SolidLine);
    }
    if(std::dynamic_pointer_cast<elem_view_and>(view)){
        draw_and(pnt, this->default_gate_w/2, this->default_gate_h/2, draw_w, draw_h); 
    }else if(std::dynamic_pointer_cast<elem_view_or>(view)){
        draw_or(pnt, this->default_gate_w/2, this->default_gate_h/2, draw_w, draw_h); 
    }else if(std::dynamic_pointer_cast<elem_view_not>(view)){
        draw_not(pnt, this->default_gate_w/2, this->default_gate_h/2, draw_w, draw_h); 
    }else if(std::dynamic_pointer_cast<elem_view_meta>(view)){
        draw_meta(pnt, this->default_gate_w/2, this->default_gate_h/2, draw_w, draw_h); 
    }else{
        QRectF rect(this->default_gate_w/2, this->default_gate_h/2, draw_w, draw_h); 
        pnt.drawRect(rect);
    }

    for(auto &gate:view->gates_in){
        pnt.drawRect(gate->x*cam.zoom, gate->y*cam.zoom,
            gate->w*cam.zoom, gate->h*cam.zoom);
        pnt.drawText(gate->x, gate->y, QString::number(gate->id));
    }
    for(auto &gate:view->gates_out){
        pnt.drawRect(gate->x*cam.zoom, gate->y*cam.zoom,
            gate->w*cam.zoom, gate->h*cam.zoom);
        pnt.drawText(gate->x, gate->y, QString::number(gate->id));
    }
    pnt.end();

    if(std::dynamic_pointer_cast<elem_view_gate>(view)){
        auto gt = sim_root->find_gate(view->id);
        auto bit_val = gt->get_values();
        QString txt;
        txt.reserve(bit_val.size());
        for(const auto &bit:bit_val){
            txt.append(bit?"1":"0");
        }
        draw_text(draw_x, draw_y, txt);
        draw_text(draw_x, draw_y-12, QString::number(view->id));
    }
    draw_widget::draw_image(draw_x, draw_y, img);
    QPen pen_valid(Qt::black);
    QPen pen_invalid(Qt::red);
    for(auto &gate:view->gates_out){
        for(auto &cn:gate->conn){
            auto &gt = cn->gate_in;
            if(cn->valid){
                draw_widget::set_pen(pen_valid);
            }else{
                draw_widget::set_pen(pen_invalid);
            }
            draw_widget::draw_line(gate->x+gate->parent->x+gate->w/2,
                gate->y+gate->parent->y+gate->h/2,
                gt->x+gt->parent->x+gt->w/2,
                gt->y+gt->parent->y+gt->h/2);
        }
    }
    draw_widget::set_pen(pen_valid);
}

sim_interface::sim_interface(QWidget* parent)
    :draw_widget(parent),
    glue(sim_ui_glue::get_instance())
{
    this->sim_root = std::make_unique<elem_meta>("root");
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &sim_interface::showContextMenu);
    setFocus();
    QWidget::setMouseTracking(true);
    mode = mode::still;
}
sim_interface::~sim_interface(){}

void sim_interface::try_tick(){
    try{
        sim_root->reset_processed();
        sim_root->process();
    }catch(std::runtime_error &e){
        QMessageBox::critical(this, "Error!", QString::fromStdString(e.what()));
    }
}

void sim_interface::delete_item_cm(){
    auto &id =this->view->id;
    glue.del_view(id);
    auto el = find_by_id(id);
    sim_root->erase(el);
    this->view = nullptr;
    emit element_selected(nullptr);
    update();
}
void sim_interface::cut_item_cm(){
    this->view->st = elem_view::state::cut;
    update();
}
void sim_interface::delete_items_cm(){
    for(auto el:selected_views){
        glue.del_view(el->id);
        auto sim_el_it = find_by_id(el->id);
        sim_root->erase(sim_el_it);
        if(el == this->view){
            this->view = nullptr;
            emit element_selected(nullptr);
        }
    }
    update();
}
void sim_interface::cut_items_cm(){
    for(auto el:selected_views){
        el->st == elem_view::state::cut;
    }
    update();
}
void sim_interface::go_up_cm(){
    auto root = glue.get_root();
    std::shared_ptr<elem_view> view = root;
    place_gates_in(view, *find_by_id(root->id));
    place_gates_out(view, *find_by_id(root->id));
    if(root->parent){
        auto meta_cast = std::dynamic_pointer_cast<elem_view_meta>(root->parent);
        dive_into_meta(meta_cast);
    }
}

void sim_interface::show_menu_for_none(const QPoint &p){
    QMenu contextMenu(tr("Menu for space"), this);
    this->cm_pos = p;

    QAction action_up("Go Up", this);

    action_up.connect(&action_up, SIGNAL(triggered()),
        this, SLOT(go_up_cm()));
    action_up.setDisabled(!glue.root_is_not_global());
    contextMenu.addAction(&action_up);
    contextMenu.exec(mapToGlobal(p));
}
void sim_interface::show_menu_for_element(const QPoint &p){
    QMenu contextMenu(tr("Menu for element"), this);
    this->cm_pos = p;

    QAction action_del("Delete item", this);
    QAction action_cut("Cut item", this);

    action_del.connect(&action_del, SIGNAL(triggered()),
        this, SLOT(delete_item_cm()));
    action_cut.connect(&action_cut, SIGNAL(triggered()),
        this, SLOT(cut_item_cm()));
    contextMenu.addAction(&action_del);
    contextMenu.addAction(&action_cut);
    contextMenu.exec(mapToGlobal(p));
}
void sim_interface::show_menu_for_elements(const QPoint &p){
    QMenu contextMenu(tr("Menu for elements"), this);
    this->cm_pos = p;

    QAction action_del("Delete items", this);
    QAction action_cut("Cut items", this);

    action_del.connect(&action_del, SIGNAL(triggered()),
        this, SLOT(delete_items_cm()));
    action_cut.connect(&action_cut, SIGNAL(triggered()),
        this, SLOT(cut_items_cm()));
    contextMenu.addAction(&action_del);
    contextMenu.addAction(&action_cut);
    contextMenu.exec(mapToGlobal(p));
}
void sim_interface::showContextMenu(const QPoint &p){
    if(selected_views.size() != 0){
        show_menu_for_elements(p);
        return;
    }
    if(this->view && this->view->st == elem_view::state::selected){
        show_menu_for_element(p);
        return;
    }
    show_menu_for_none(p);
}

void sim_interface::mouseMoveEvent(QMouseEvent *e){
    setFocus();
    this->mouse_pos_prev_move = this->mouse_pos;
    this->mouse_pos = e->pos();
    auto x = e->x();
    auto y = e->y();

    if(mode == mode::create){
        view->x = x;
        view->y = y;
        emit element_selected(view);
        update();
        return;
    }
    if(mode == mode::select){
        if(e->buttons() & Qt::LeftButton){
            auto predicate = [](auto view){
                return view->st == elem_view::state::selected;
            };
            auto views = glue.find_views(predicate);
            auto trans = [this](auto view){
                auto dx = (*mouse_pos).x()-(*mouse_pos_prev_move).x();
                view->x += dx*1.;
                auto dy = (*mouse_pos).y()-(*mouse_pos_prev_move).y();
                view->y += dy*1.;
                return view;
            };
            std::transform(views.begin(), views.end(), views.begin(), trans);
            if(views.size() > 1){
                emit element_selected(nullptr);
            }else{
                emit element_selected(views.at(0));
            }
            update();
            return;
        }
    }
    if(this->mouse_pos_prev.has_value() && this->mode == mode::still){//click happened
        this->mode = mode::select_frame;
    }
    if(mode == mode::select_frame){
        this->selected_views = glue.find_views(mouse_pos_prev->x(), mouse_pos_prev->y(),
            mouse_pos->x() - mouse_pos_prev->x(),
            mouse_pos->y() - mouse_pos_prev->y());
        for(auto &v:selected_views){
            v->st = elem_view::state::selected;
        }
        update();//draw frame
    }
}

void sim_interface::dive_into_meta(std::shared_ptr<elem_view_meta> value){
    glue.set_root(value);
    this->view = nullptr;
    emit element_selected(nullptr);
    this->selected_views.clear();
    this->mode = mode::still;
    update();
}
void sim_interface::set_in_value(std::shared_ptr<elem_view_in> view){
    int input = QInputDialog::getInt(this, "input devimal value", "input decimal value to pass");
    auto bits = bits::to_bits(input);
    class elem_in* elem_in;
    try{
        auto &ref = sim_root->find_element(view->id);
        elem_in = dynamic_cast<class elem_in*>(ref.get());
    }catch(std::out_of_range &e){
        qWarning()<<"something went wrong:"<<e.what();
        return;
    }
    auto out_width = elem_in->get_width();
    if(bits.size() > out_width){
        //TODO:warning
        bits.erase(bits.begin()+out_width, bits.end());
    }else if(bits.size() < out_width){
        bits.resize(out_width);
    }
    elem_in->set_values(bits);
}

void sim_interface::mousePressEvent(QMouseEvent *e){
    this->mouse_pos_prev = e->pos();
    auto x = e->x();
    auto y = e->y();
    auto map_x = (x-cam.x)*1./cam.zoom;
    auto map_y = (y-cam.y)*1./cam.zoom;

    //place created element or destroy it
    if(this->view && mode == mode::create){
        if(e->buttons() & Qt::LeftButton){
            glue.add_view(view);
            mode = mode::select;
            view->st = elem_view::state::selected;
            emit element_selected(view);
        }else if(e->buttons() & Qt::RightButton){
            auto el_it = find_by_id(this->view->id);
            sim_root->erase(el_it);
            view = nullptr;
            mode = mode::still;
        }
        update();
        return;
    }
    if(e->buttons() & Qt::LeftButton){
        //check if user clicked on a view of element
        auto items = glue.find_views(map_x, map_y);
        if(!items.empty()){
            if(this->view == items.front() && mode == mode::select){
                auto cast_in = std::dynamic_pointer_cast<elem_view_in>(view);
                if(cast_in){
                    set_in_value(cast_in);
                    try_tick();
                    return;
                }
                auto cast_meta = std::dynamic_pointer_cast<elem_view_meta>(view);
                if(cast_meta){
                    dive_into_meta(cast_meta);
                    return;
                }
            }else{
                this->view = items.front();
                this->view->st = elem_view::state::selected;
                this->mode = mode::select;
                emit element_selected(view);
                return;
            }
        }else{
            //check if user clicked on a gate
            auto gates = glue.get_gates(map_x, map_y);
            if(!gates.empty()){
                this->mode = mode::connect_gates;
                this->gate_view_1 = gates.front();
                return;
            }
        }
        //deselect element
        mode = mode::still;
        if(this->view){
            this->view->st = elem_view::state::normal;
            this->view = nullptr;
        }
        for(auto &v:selected_views){
            v->st = elem_view::state::normal;
        }
        selected_views.clear();
    }
}

void sim_interface::mouseReleaseEvent(QMouseEvent *e){
    auto x = e->x();
    auto y = e->y();
    auto reset_pos = [this](){
        this->mouse_pos.reset();
        this->mouse_pos_prev.reset();
        this->mouse_pos_prev_move.reset();
    };
    if(this->mode == mode::still){
        reset_pos();
        return;
    }
    if(this->mode == mode::select && view){
        reset_pos();
        return;
    }
    if(this->mode == mode::connect_gates && this->gate_view_1) {
        auto gates = glue.get_gates(x, y);
        if(gates.empty()){
            return;
        }
        this->gate_view_2 = gates.front();
        connect_gates(this->gate_view_1, this->gate_view_2);
        try_tick();
        this->gate_view_1 = this->gate_view_2 = nullptr;
        this->mode = mode::still;
        reset_pos();
        return;
    }
    if(this->mode == mode::select_frame){
        reset_pos();
        this->mode = mode::still;
        return;
    }
}

void sim_interface::keyPressEvent(QKeyEvent* e){
    pressed_keys.emplace_back(e->key());
    auto beg = pressed_keys.cbegin();
    auto end = pressed_keys.cend();
    bool ctrl = std::find(beg, end, Qt::Key_Control) != end;
    bool left = std::find(beg, end, Qt::Key_Left) != end;
    bool right = std::find(beg, end, Qt::Key_Right) != end;
    bool up = std::find(beg, end, Qt::Key_Up) != end;
    bool down = std::find(beg, end, Qt::Key_Down) != end;
    bool plus = std::find(beg, end, Qt::Key_Equal) != end;
    bool minus = std::find(beg, end, Qt::Key_Minus) != end;
    bool x = std::find(beg, end, Qt::Key_X) != end;
    if(this->view){
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
        if(plus){
            cam.zoom += 0.25;
            return;
        }else if(minus){
            cam.zoom -= 0.25;
            return;
        }
    }
}

void sim_interface::keyReleaseEvent(QKeyEvent* e){
    auto it = std::find(pressed_keys.begin(), pressed_keys.end(), e->key());
    if(it != pressed_keys.end()){
        this->pressed_keys.erase(it);
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
        draw_elem_view(pnt, el);
    }
    if(mode == mode::create){
        draw_elem_view(pnt, this->view);
    }else if(mode == mode::connect_gates &&
        mouse_pos_prev.has_value() && mouse_pos.has_value())
    {
        draw_line(mouse_pos->x(), mouse_pos->y(),
            mouse_pos_prev->x(), mouse_pos_prev->y());
    }else if(mode == mode::select_frame){
        draw_rect(mouse_pos_prev->x(), mouse_pos_prev->y(),
            mouse_pos->x() - mouse_pos_prev->x(),
            mouse_pos->y() - mouse_pos_prev->y());
    }
    this->draw_widget::paintEvent(e);
}

void sim_interface::slot_propery_changed(const prop_pair* prop){
    if(this->view && !prop->get_line_edit()->text().isEmpty()){
        prop->set_view_value(this->view);
        auto gate_cast = std::dynamic_pointer_cast<gate_view>(view);
        if(gate_cast && prop->name() == "bit_w"){
            auto gt = sim_root->find_gate(view->id);
            gt->set_width(gate_cast->bit_width);
            auto func = [this, &gate_cast](std::shared_ptr<gate_view> gt){
                gt->bit_width = gate_cast->bit_width;
                for(auto &cn:gt->conn){
                    bool prev = cn->valid;
                    cn->check_valid();
                    bool now = cn->valid;
                    if(!prev && now){
                        auto gt2 = cn->gate_in == gt?
                            std::static_pointer_cast<gate_view>(cn->gate_out):
                            std::static_pointer_cast<gate_view>(cn->gate_in);
                        connect_gates(gt, gt2);
                    }
                }
            };
            for(auto &gt_out:view->gates_out){
                func(gt_out);
            }
            for(auto &gt_in:view->gates_in){
                func(gt_in);
            }
        }
        update();
    }
}

void sim_interface::add_elem_and(){
    create_elem<elem_and>("and");
}
void sim_interface::add_elem_or(){
    create_elem<elem_or>("or");
}
void sim_interface::add_elem_not(){
    create_elem<elem_not>("not");
}
void sim_interface::add_elem_in(){
    create_elem<elem_in>("in");
}
void sim_interface::add_elem_out(){
    create_elem<elem_out>("out");
}
void sim_interface::add_elem_meta(){
    create_elem<elem_meta>("custom");
}