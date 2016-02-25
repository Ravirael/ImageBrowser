#pragma once

#include <QMouseEvent>

class EventDependantI
{
public:
    virtual ~EventDependantI(){}

    virtual void resizeEvent(QResizeEvent *event) = 0;

    virtual void mousePressEvent(QMouseEvent *event) = 0;

    virtual void mouseMoveEvent(QMouseEvent *event) = 0;

    virtual void wheelEvent(QWheelEvent *event) = 0;



signals:

public slots:
};

