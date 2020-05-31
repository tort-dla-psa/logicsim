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
	prop_pair(const QString &name, const QString &lbl_txt, const QString &default_value, QWidget *parent);
	prop_pair(const QString &name, const QString &lbl_txt, QWidget *parent);
	~prop_pair();

	const QLabel* get_label()const;
	QLabel* get_label();
	const QLineEdit* get_line_edit()const;
	QString get_value()const;
	QLineEdit* get_line_edit();
	const QString& name()const;
public slots:
	void reset();
signals:
	void text_changed();
private slots:
	void slot_text_changed(QString txt);
};
