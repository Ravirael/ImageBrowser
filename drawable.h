#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPointF>

class Drawable : public QObject
{
    Q_OBJECT

protected:

public:
    explicit Drawable(QObject *parent = 0);
    virtual ~Drawable();

signals:
    void requestUpdate();

public slots:
    virtual void draw(QWidget *canvas) = 0;
};

