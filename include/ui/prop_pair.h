#pragma once

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <memory>
#include <functional>

class view;
class prop_pair: public QWidget{
	Q_OBJECT

	QLabel* lbl;
	QLineEdit* le;
	QHBoxLayout* layout;
	QString m_name;
	const QString default_value;
public:
	using prop_func_get = std::function<QString(const std::shared_ptr<view>)>;
	using prop_func_set = std::function<void(std::shared_ptr<view>)>;
	prop_pair(const QString &name, const QString &lbl_txt, const QString &default_value, QWidget *parent);
	prop_pair(const QString &name, const QString &lbl_txt, QWidget *parent);
	~prop_pair();

	const QLabel* get_label()const;
	QLabel* get_label();
	const QLineEdit* get_line_edit()const;
	QLineEdit* get_line_edit();

	void set_line_edit_value(std::shared_ptr<view> view);
	void set_view_value(std::shared_ptr<view> view)const;

	void set_getter(prop_func_get getter);
	void set_setter(prop_func_set setter);
	const QString& name()const;

public slots:
	void reset();
signals:
	void text_changed();
private slots:
	void slot_text_changed(QString txt);
private:
	std::optional<prop_func_get> getter;
	std::optional<prop_func_set> setter;
};
