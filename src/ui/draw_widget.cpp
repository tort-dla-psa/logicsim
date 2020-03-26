#include <QPainter>
#include "draw_widget.h"

draw_widget::draw_widget(QWidget *parent)
	:QWidget(parent)
{
	img = QImage(this->size(), QImage::Format::Format_RGB32);
	pn = QPen(Qt::black,1);
	clean();
}

void draw_widget::set_instrument(QPainter &pntr){
	if(use_br){
		pntr.setBrush(br);
	}else{
		pntr.setPen(pn);
	}
}

void draw_widget::clean() {
	img.fill(Qt::white);
	update();
}

void draw_widget::resizeEvent(QResizeEvent *e) {
	if (width() > img.width() ||
		height() > img.height())
	{
		int newWidth = qMax(width() + 128, img.width());
		int newHeight = qMax(height() + 128, img.height());
		resize_image(&img, QSize(newWidth, newHeight));
		update();
	}
	QWidget::resizeEvent(e);
}

void draw_widget::paintEvent(QPaintEvent *e) {
	QPainter p(this);
	p.drawImage(e->rect(), img, e->rect());
}

void draw_widget::resize_image(QImage *image, const QSize &newSize) {
	if (image->size() == newSize)
		return;
	QImage newImage(newSize, QImage::Format_RGB32);
	newImage.fill(Qt::white);
	QPainter painter(&newImage);
	painter.drawImage(QPoint(0, 0), *image);
	painter.endNativePainting();
	*image = newImage;
	update();
}

void draw_widget::set_pen(QPen pn){
	this->pn = pn;
}
void draw_widget::set_brush(QBrush br){
	this->br = br;
}
void draw_widget::use_brush(bool flag){
	this->use_br = flag;
}
void draw_widget::draw_line(int x1, int y1, int x2, int y2){
	QPainter painter(&img);
	set_instrument(painter);
	painter.drawLine(x1,y1,x2,y2);
}
void draw_widget::draw_rect(int x, int y, int w, int h){
	QPainter painter(&img);
	set_instrument(painter);
	painter.drawRect(x, y, w, h);
}
void draw_widget::draw_ellipse(int x, int y, int w, int h){
	QPainter painter(&img);
	set_instrument(painter);
	painter.drawEllipse(QPointF(x, y), w, h);
}
void draw_widget::draw_text(QPoint pn, QString str){
	QPainter painter(&img);
	set_instrument(painter);
	painter.drawText(pn, str);
}
void draw_widget::draw_text(int x, int y, QString str){
	draw_text(QPoint(x, y), str);
}
void draw_widget::draw_image(int x, int y, const QImage& img){
	QPainter painter(&this->img);
	painter.drawImage(x, y, img);
}