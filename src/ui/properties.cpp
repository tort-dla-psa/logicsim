#include "properties.h"
#include "ui_properties.h"
#include "sim_ui_glue.h"

properties::properties(QWidget* parent)
    :QWidget(parent), ui(new Ui::properties)
{
    ui->setupUi(this);
}
properties::~properties(){
    delete ui;
}
void properties::slot_element_selected(std::shared_ptr<elem_view> view){
    ui->edit_x->setText(QString::number(view->x));
    ui->edit_y->setText(QString::number(view->y));
    ui->edit_w->setText(QString::number(view->w));
    ui->edit_h->setText(QString::number(view->h));
}