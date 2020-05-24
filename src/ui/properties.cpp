#include "properties.h"
#include "prop_pair.h"
#include "ui_properties.h"
#include "sim_ui_glue.h"

properties::properties(QWidget* parent)
    :QWidget(parent), ui(new Ui::properties)
{
    ui->setupUi(this);
    this->scroll_layout = new QVBoxLayout(ui->scrollArea);
    ui->scrollArea->setLayout(this->scroll_layout);

    auto prop = props.emplace_back(new prop_pair("name", "Name", this));
    prop->set_getter([](auto view){
        return QString::fromStdString(view->name);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->name = le->text().toStdString();
    }); 

    prop = props.emplace_back(new prop_pair("x", "X", this));
    prop->set_getter([](auto view){
        return QString::number(view->x);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->x = le->text().toLong();
    }); 

    prop = props.emplace_back(new prop_pair("y", "Y", this));
    prop->set_getter([](auto view){
        return QString::number(view->y);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->y = le->text().toLong();
    }); 

    prop = props.emplace_back(new prop_pair("w", "W", this));
    prop->set_getter([](auto view){
        return QString::number(view->w);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->w = le->text().toLong();
    }); 

    prop = props.emplace_back(new prop_pair("h", "H", this));
    prop->set_getter([](auto view){
        return QString::number(view->h);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->h = le->text().toLong();
    }); 

    prop = props.emplace_back(new prop_pair("bit_w", "bits", this));
    prop->set_getter([](auto view){
        auto cast_in = std::dynamic_pointer_cast<elem_view_in>(view);
        if(cast_in){
            return QString::number(cast_in->outs.at(0)->bit_width);
        }
        auto cast_out = std::dynamic_pointer_cast<elem_view_out>(view);
        if(cast_out){
            return QString::number(cast_out->ins.at(0)->bit_width);
        }
        auto gate_cast = std::dynamic_pointer_cast<gate_view>(view);
        if(gate_cast){
            return QString::number(gate_cast->bit_width);
        }
        return QString::number(0);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        auto data = le->text().toLong();
        auto cast_in = std::dynamic_pointer_cast<elem_view_in>(view);
        if(cast_in){
            cast_in->outs.at(0)->bit_width = data;
            return;
        }
        auto cast_out = std::dynamic_pointer_cast<elem_view_out>(view);
        if(cast_out){
            cast_out->ins.at(0)->bit_width = data;
            return;
        }
        auto gate_cast = std::dynamic_pointer_cast<gate_view>(view);
        if(gate_cast){
            gate_cast->bit_width = data;
        }
    }); 
    prop->hide();

    prop = props.emplace_back(new prop_pair("value", "Value", this));
    prop->set_getter([](auto view){
        auto cast_gate = std::dynamic_pointer_cast<gate_view>(view);
        if(cast_gate){
            return QString::number(cast_gate->value);
        }
        auto cast_view_in = std::dynamic_pointer_cast<elem_view_in>(view);
        if(cast_view_in){
            return QString::number(cast_view_in->outs.at(0)->value);
        }
        auto cast_view_out = std::dynamic_pointer_cast<elem_view_out>(view);
        if(cast_view_out){
            return QString::number(cast_view_out->ins.at(0)->value);
        }
        return QString::number(0);
    });
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        auto cast_in = std::dynamic_pointer_cast<elem_view_in>(view);
        if(cast_in){
            cast_in->outs.at(0)->bit_width = le->text().toLong();
            return;
        }
    });
    prop->hide();

    for(auto prop:props){
        this->scroll_layout->addWidget(prop);
        connect(prop, &prop_pair::text_changed, 
            this, &properties::slot_prop_changed);
    }
}
properties::~properties(){
    delete ui;
    delete scroll_layout;
    for(auto prop:props){
        delete prop;
    }
    props.clear();
}

void properties::update_props(const std::shared_ptr<view> &view){
    for(auto prop:props){
        auto name = prop->name(); 
        if(name == "name" ||
            name == "x" ||
            name == "y" ||
            name == "w" ||
            name == "h")
        {
            prop->show();
        }else if(name == "bit_w"){
            if(std::dynamic_pointer_cast<elem_view_gate>(view)||
                std::dynamic_pointer_cast<gate_view>(view))
            {
                prop->show();
            }else{
                prop->hide();
            }
        }else if(name == "value"){
            if(std::dynamic_pointer_cast<gate_view>(view) ||
                std::dynamic_pointer_cast<elem_view_gate>(view)){
                prop->show();
            }else{
                prop->hide();
            }
        }else{
            prop->hide();
        }
    }
}

void properties::reset(){
    for(auto prop:props){
        prop->reset();
    }
}
void properties::slot_element_selected(std::shared_ptr<view> view){
    if(!view){
        reset();
        return;
    }
    update_props(view);
    for(auto &prop:props){
        if(prop->isHidden()){
            continue;
        }
        prop->set_line_edit_value(view);
    }
}
void properties::slot_prop_changed(){
    auto sender = (prop_pair*)QObject::sender();
    emit property_changed(sender);
}
