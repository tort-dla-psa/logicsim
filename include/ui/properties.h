#pragma once

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <memory>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class properties; }
QT_END_NAMESPACE

class view;
class prop_pair;

class properties : public QWidget {
	Q_OBJECT

	Ui::properties *ui;
	QLayout* scroll_layout;
	std::vector<prop_pair*> props;

	void update_props();
	void update_props(const std::shared_ptr<class view> &view);
public:
	properties(QWidget *parent = nullptr);
	virtual ~properties();
signals:
	void property_changed(const prop_pair* prop);
public slots:
	void slot_element_selected(std::shared_ptr<class view> view);
	void reset();
private slots:
	void slot_prop_changed();
};
