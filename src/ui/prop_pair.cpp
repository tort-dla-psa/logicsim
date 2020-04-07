#include "prop_pair.h"
#include "sim_ui_glue.h"

prop_pair::prop_pair(QString name, QString lbl_txt, QWidget *parent)
    :QWidget(parent),
    m_name(name)
{
    this->layout = new QHBoxLayout(this);
    this->lbl = new QLabel(this);
    this->lbl->setText(lbl_txt);
    this->le = new QLineEdit(this);
    this->layout->addWidget(this->lbl);
    this->layout->addWidget(this->le);
    connect(this->le, &QLineEdit::textEdited,
        this, &prop_pair::slot_text_changed);
    hidden = false;
}
prop_pair::~prop_pair(){
    delete this->le;
    delete this->lbl;
    delete this->layout;
}
QString prop_pair::name()const{
    return m_name;
}
void prop_pair::set_view_value(std::shared_ptr<elem_view> view)const{
    if(setter.has_value()){
        (*setter)(view);
    }
}
void prop_pair::set_line_edit_value(std::shared_ptr<elem_view> view){
    if(getter.has_value()){
        this->le->setText((*getter)(view));
    }
}
const QLabel* prop_pair::get_label()const{
    return this->lbl;
}
QLabel* prop_pair::get_label(){
    return this->lbl;
}
const QLineEdit* prop_pair::get_line_edit()const{
    return this->le;
}
QLineEdit* prop_pair::get_line_edit(){
    return this->le;
}
void prop_pair::set_getter(prop_func_get getter){
    this->getter.emplace(getter);
}
void prop_pair::set_setter(prop_func_set setter){
    this->setter.emplace(setter);
}
void prop_pair::slot_text_changed(QString txt){
    emit text_changed();
}
void prop_pair::hide(){
    hidden = true;
}
void prop_pair::show(){
    hidden = false;
}
bool prop_pair::is_hidden()const{
    return this->hidden;
}
