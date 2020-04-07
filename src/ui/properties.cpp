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
        return QString::number(view->bit_width);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->bit_width = le->text().toLong();
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

void properties::update_props(const std::shared_ptr<elem_view> &view){
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
            if(view->t == elem_view::type::type_in ||
                view->t == elem_view::type::type_out)
            {
                prop->QWidget::setHidden(false);
            }else{
                prop->QWidget::setHidden(true);
            }
        }
    }
}

void properties::slot_element_selected(std::shared_ptr<elem_view> view){
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