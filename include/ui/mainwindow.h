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
	void open_action();
	void save_action();
signals:
	void open_signal(QString path);
	void save_signal(QString path);
};