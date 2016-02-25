#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <vector>

#include "drawable.h"
#include "eventdependanti.h"

class Canvas : public QWidget
{
    Q_OBJECT

    std::vector<Drawable*> mDrawables;
    std::vector<EventDependantI*> mEventDependats;

public:
    explicit Canvas(QWidget *parent = 0);

signals:

public slots:
    void addDrawable(Drawable &item);
    void addFollower(EventDependantI &item);

    bool discardDrawable(const Drawable &item);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);


};

