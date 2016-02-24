#pragma once

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QWidget>

#include <memory>

class ImagePainter : public QWidget
{
    Q_OBJECT
public:
    explicit ImagePainter(QWidget *parent = 0);

signals:
    void sizeChanged(QSize);
    void zoomed();

public slots:
    void setPixmap(std::shared_ptr<QPixmap>, bool reload);
    void reset();
    void calibrate();

private:
    std::shared_ptr<QPixmap> pixmap;
    double scale;
    QPoint positionOrigin, position, mouseOrigin;


protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

};
