#include "canvas.h"


Canvas::Canvas(QWidget *parent) :
    QWidget(parent)
{

}

void Canvas::paintEvent(QPaintEvent *)
{
    for (auto &d : mDrawables)
    {
        d->draw(this);
    }
}

void Canvas::resizeEvent(QResizeEvent *event)
{
    for (auto &f : mEventDependats)
    {
        f->resizeEvent(event);
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    for (auto &f : mEventDependats)
    {
        f->mousePressEvent(event);
    }

    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    for (auto &f : mEventDependats)
    {
        f->mouseMoveEvent(event);
    }

    update();
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    for (auto &f : mEventDependats)
    {
        f->wheelEvent(event);
    }

    update();
}

void Canvas::addDrawable(Drawable &item)
{
    mDrawables.push_back(&item);
    connect(&item, SIGNAL(requestUpdate()), this, SLOT(update()));
    update();
}

void Canvas::addFollower(EventDependantI &item)
{
    mEventDependats.push_back(&item);
}

bool Canvas::discardDrawable(const Drawable &item)
{
    mDrawables.erase(std::find(mDrawables.begin(), mDrawables.end(), &item));
    return true;
}
