#pragma once

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <memory>
#include <functional>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class properties; }
QT_END_NAMESPACE

class elem_view;

class prop: public QWidget{
	Q_OBJECT

	QLabel* lbl;
	QLineEdit* le;
	QHBoxLayout* layout;
public:
	using prop_func_get = std::function<QString(std::shared_ptr<elem_view>)>;
	using prop_func_set = std::function<void(std::shared_ptr<elem_view>)>;
	prop(QString lbl_txt, QWidget *parent);
	~prop();

	const QLabel* get_label()const;
	QLabel* get_label();
	const QLineEdit* get_line_edit()const;
	QLineEdit* get_line_edit();

	void set_line_edit_value(std::shared_ptr<elem_view> view);
	void set_view_value(std::shared_ptr<elem_view> view)const;

	void set_getter(prop_func_get getter);
	void set_setter(prop_func_set setter);
signals:
	void text_changed();
private slots:
	void slot_text_changed(QString txt);
private:
	std::optional<prop_func_get> getter;
	std::optional<prop_func_set> setter;
};

class properties : public QWidget {
	Q_OBJECT

	Ui::properties *ui;
	QLayout* scroll_layout;
	std::vector<::prop*> props;
public:
	properties(QWidget *parent = nullptr);
	virtual ~properties();
signals:
	void property_changed(const class prop* prop);
public slots:
	void slot_element_selected(std::shared_ptr<elem_view> view);
private slots:
	void slot_prop_changed();
};