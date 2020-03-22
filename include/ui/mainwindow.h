#pragma once
#include <QMainWindow>
#include "sim_interface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private:
	sim_interface *sim_iface;
	Ui::MainWindow *ui;
private slots:
};