#include "ratingpainter.h"

#include <QImage>
#include <QIcon>

RatingPainter::RatingPainter(QObject *parent):Drawable(parent),currentRating(RatingSystem::Medium)
{

    icons[RatingSystem::Highest] = QPixmap("://icons/mysmile.png");

    icons[RatingSystem::Medium] = QPixmap("://icons/myunsure.png");

    icons[RatingSystem::Lowest] = QPixmap("://icons/mysad.png");

    timer.setSingleShot(true);
    timer.setInterval(400);
    fadeTimer.setInterval(20);

    connect(&timer, SIGNAL(timeout()), this, SLOT(fade()));
    connect(&fadeTimer, SIGNAL(timeout()), this, SLOT(fade()));

}

RatingPainter::~RatingPainter()
{

}

QPixmap RatingPainter::getPixmap() const
{
    return getPixmap(currentRating);
}

QPixmap RatingPainter::getPixmap(RatingSystem::Rating rating) const
{
    return icons.at(rating);
}

void RatingPainter::draw(QWidget *canvas)
{
    QPainter painter(canvas);
    painter.setOpacity(opacity);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(size.width()/2 - 32, size.height() - 80, 64, 64, icons[currentRating]);
}


void RatingPainter::display()
{
    opacity = 0.75;
    timer.stop();

    fadeTimer.stop();

    timer.start();
    emit requestUpdate();
}

void RatingPainter::fade()
{
    opacity -= 0.04;

    if (!fadeTimer.isActive())
    {
        fadeTimer.start();
    }

    if (opacity <= 0)
    {
        fadeTimer.stop();
    }

    emit requestUpdate();
}

void RatingPainter::setRating(RatingSystem::Rating rating)
{
   // opacity = 0.75;
   // timer.start(2000);
    currentRating = rating;
    emit requestUpdate();
}

void RatingPainter::resizeEvent(QResizeEvent *event)
{
    size = event->size();
}
