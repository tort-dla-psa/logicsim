#include "mainwindow.h"
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
}

MainWindow::~MainWindow(){
	delete ui;
}