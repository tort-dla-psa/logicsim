#pragma once

#include <QObject>
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class properties; }
QT_END_NAMESPACE

class elem_view;
class properties : public QWidget {
	Q_OBJECT

	Ui::properties *ui;
public:
	properties(QWidget *parent = nullptr);
	virtual ~properties();
signals:
public slots:
	void slot_element_selected(std::shared_ptr<elem_view> view);
};