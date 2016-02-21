#pragma once

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QWidget>

class ImagePainter : public QWidget
{
    Q_OBJECT
public:
    explicit ImagePainter(QWidget *parent = 0);
    bool openFile(QString fileName);

signals:
    void sizeChanged(QSize);
    void zoomed();

public slots:
    void setPixmap(QPixmap *);
    void reset();
    void calibrate();

private:
    QPixmap *pixmap;
    double scale;
    QPoint positionOrigin, position, mouseOrigin;


protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

};
