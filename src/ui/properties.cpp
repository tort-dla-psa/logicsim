#include "properties.h"
#include "ui_properties.h"
#include "sim_ui_glue.h"

prop::prop(QString lbl_txt, QWidget *parent)
    :QWidget(parent)
{
    this->layout = new QHBoxLayout(this);
    this->lbl = new QLabel(this);
    this->lbl->setText(lbl_txt);
    this->le = new QLineEdit(this);
    this->layout->addWidget(this->lbl);
    this->layout->addWidget(this->le);
    connect(this->le, &QLineEdit::textEdited,
        this, &prop::slot_text_changed);
}
prop::~prop(){
    delete this->le;
    delete this->lbl;
    delete this->layout;
}
void prop::set_view_value(std::shared_ptr<elem_view> view)const{
    if(setter.has_value()){
        (*setter)(view);
    }
}
void prop::set_line_edit_value(std::shared_ptr<elem_view> view){
    if(getter.has_value()){
        this->le->setText((*getter)(view));
    }
}
const QLabel* prop::get_label()const{
    return this->lbl;
}
QLabel* prop::get_label(){
    return this->lbl;
}
const QLineEdit* prop::get_line_edit()const{
    return this->le;
}
QLineEdit* prop::get_line_edit(){
    return this->le;
}
void prop::set_getter(prop_func_get getter){
    this->getter.emplace(getter);
}
void prop::set_setter(prop_func_set setter){
    this->setter.emplace(setter);
}
void prop::slot_text_changed(QString txt){
    emit text_changed();
}

properties::properties(QWidget* parent)
    :QWidget(parent), ui(new Ui::properties)
{
    ui->setupUi(this);
    this->scroll_layout = new QVBoxLayout(ui->scrollArea);
    ui->scrollArea->setLayout(this->scroll_layout);

    auto prop = props.emplace_back(new class prop("Name", this));
    prop->set_getter([](auto view){
        return QString::fromStdString(view->name);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->name = le->text().toStdString();
    }); 

    prop = props.emplace_back(new class prop("X", this));
    prop->set_getter([](auto view){
        return QString::number(view->x);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->x = le->text().toLong();
    }); 

    prop = props.emplace_back(new class prop("Y", this));
    prop->set_getter([](auto view){
        return QString::number(view->y);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->y = le->text().toLong();
    }); 

    prop = props.emplace_back(new class prop("W", this));
    prop->set_getter([](auto view){
        return QString::number(view->w);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->w = le->text().toLong();
    }); 

    prop = props.emplace_back(new class prop("H", this));
    prop->set_getter([](auto view){
        return QString::number(view->h);
    }); 
    prop->set_setter([prop](auto view){
        auto le = prop->get_line_edit();
        view->h = le->text().toLong();
    }); 

    for(auto prop:props){
        this->scroll_layout->addWidget(prop);
        connect(prop, &prop::text_changed, 
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
void properties::slot_element_selected(std::shared_ptr<elem_view> view){
    for(auto &prop:props){
        prop->set_line_edit_value(view);
    }
}
void properties::slot_prop_changed(){
    auto sender = (prop*)QObject::sender();
    emit property_changed(sender);
}