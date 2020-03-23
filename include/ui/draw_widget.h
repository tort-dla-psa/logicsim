#pragma once

#include <QObject>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class draw_widget : public QWidget {
	Q_OBJECT

	QImage img;
	QPen pn;
	QBrush br;
	bool use_br;

	void resizeEvent(QResizeEvent *e)override;

	void resize_image(QImage *image, const QSize &newSize);
	void set_instrument(QPainter &pntr);
public:
	draw_widget(QWidget *parent = nullptr);
	virtual ~draw_widget(){}

	void paintEvent(QPaintEvent *e)override;
signals:
public slots:
	void clean();
	void set_pen(QPen pn);
	void set_brush(QBrush br);
	void use_brush(bool flag);
	void draw_line(int x1, int y1, int x2, int y2);
	void draw_rect(int x, int y, int w, int h);
	void draw_ellipse(int x, int y, int w, int h);
	void draw_text(QPoint pn, QString str);
	void draw_image(int x, int y, const QImage& img);
};