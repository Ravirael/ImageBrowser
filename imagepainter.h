#pragma once

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QWidget>

#include "drawable.h"
#include "eventdependanti.h"

#include <memory>

class ImagePainter : public Drawable, public EventDependantI
{
    Q_OBJECT

    QSize size;
public:
    explicit ImagePainter(QWidget *parent = 0);

signals:
    void sizeChanged(QSize);
    void zoomed();

public slots:
    void setPixmap(QPixmap, bool reload);
    void reset();
    void calibrate();

    void draw(QWidget *canvas) override;


private:
    QPixmap pixmap;
    double scale;
    QPoint positionOrigin, position, mouseOrigin;


public:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

};
