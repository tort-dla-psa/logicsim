#include "mainwindow.h"
#include "prop_pair.h"
#include "ui_mainwindow.h"
#include "sim/basic_elements.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->sim_iface = ui->sim_view_wdgt;
	connect(ui->btn_and, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_and);
	connect(ui->btn_or, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_or);
	connect(ui->btn_not, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_not);
	connect(ui->btn_in, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_in);
	connect(ui->btn_out, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_out);
	connect(ui->btn_meta, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_meta);
	connect(ui->sim_view_wdgt, &sim_interface::element_selected,
		ui->props, &properties::slot_element_selected);
	connect(ui->props, &properties::property_changed,
		ui->sim_view_wdgt, &sim_interface::slot_propery_changed);
}

MainWindow::~MainWindow(){
	delete ui;
}