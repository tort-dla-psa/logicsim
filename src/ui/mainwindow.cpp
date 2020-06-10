#include <QFileDialog>
#include <QMessageBox>
#include "ui/mainwindow.h"
#include "ui/prop_pair.h"
#include "ui_mainwindow.h"
#include "sim/basic_elements.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->sim_iface = ui->sim_view_wdgt;
	connect(ui->btn_and, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_and);
	connect(ui->btn_nand, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_nand);
	connect(ui->btn_or, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_or);
	connect(ui->btn_nor, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_nor);
	connect(ui->btn_xor, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_xor);
	connect(ui->btn_xnor, &QPushButton::pressed,
		sim_iface, &sim_interface::add_elem_xnor);
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
	connect(ui->actionOpen, &QAction::triggered,
		this, &MainWindow::open_action);
	connect(ui->actionSave, &QAction::triggered,
		this, &MainWindow::save_action);
	connect(this, &MainWindow::open_signal,
		sim_iface, &sim_interface::load_sim);
	connect(this, &MainWindow::save_signal,
		sim_iface, &sim_interface::save_sim);
}

MainWindow::~MainWindow(){
	delete ui;
}

void MainWindow::open_action(){
	this->setFocus();
	auto path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),
		tr("Sim (*.sim);;All Files (*)"), nullptr, QFileDialog::DontUseNativeDialog);
	if(path.isNull() || path.isEmpty()){
		return;
	}
	emit open_signal(path);
}
void MainWindow::save_action(){
	auto path = QFileDialog::getSaveFileName(this, "Save file", QDir::currentPath(),
		tr("Sim (*.sim);;All Files (*)"), nullptr, QFileDialog::DontUseNativeDialog);
	emit save_signal(path);
}
