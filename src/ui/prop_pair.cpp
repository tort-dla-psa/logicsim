#include "prop_pair.h"
#include "sim_ui_glue.h"

prop_pair::prop_pair(const QString &name, const QString &lbl_txt, QWidget *parent):
    prop_pair(name, lbl_txt, "", parent)
{}

prop_pair::prop_pair(const QString &name, const QString &lbl_txt,
    const QString &default_value, QWidget *parent)
    :QWidget(parent),
    m_name(name),
    default_value(default_value)
{
    this->layout = new QHBoxLayout(this);
    this->lbl = new QLabel(this);
    this->lbl->setText(lbl_txt);
    this->le = new QLineEdit(this);
    this->layout->addWidget(this->lbl);
    this->layout->addWidget(this->le);
    connect(this->le, &QLineEdit::textEdited,
        this, &prop_pair::slot_text_changed);
}
prop_pair::~prop_pair(){
    delete this->le;
    delete this->lbl;
    delete this->layout;
}
void prop_pair::reset(){
    this->le->setText(default_value);
}
const QString& prop_pair::name()const{
    return m_name;
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
QString prop_pair::get_value()const{
    return this->le->text();
}
void prop_pair::slot_text_changed(QString txt){
    emit text_changed();
}
