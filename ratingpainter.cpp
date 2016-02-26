#include "ratingpainter.h"

#include <QImage>

RatingPainter::RatingPainter(QObject *parent):Drawable(parent),currentRating(RatingSystem::Medium)
{

    icons[RatingSystem::Highest] = QPixmap("://icons/mysmile.png");

    icons[RatingSystem::Medium] = QPixmap("://icons/myunsure.png");

    icons[RatingSystem::Lowest] = QPixmap("://icons/mysad.png");

    connect(&timer, SIGNAL(timeout()), this, SLOT(fade()));

}

RatingPainter::~RatingPainter()
{

}

void RatingPainter::draw(QWidget *canvas)
{
    QPainter painter(canvas);
    painter.setOpacity(opacity);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(size.width()/2 - 32, size.height() - 80, 64, 64, icons[currentRating]);
}

void RatingPainter::drawMinature(QIcon *icon)
{

}

void RatingPainter::fade()
{
    opacity -= 1;

    if (opacity <= 0)
    {
        timer.stop();
    }


    emit requestUpdate();
}

void RatingPainter::setRating(RatingSystem::Rating rating)
{
    opacity = 0.75;
    timer.start(2000);
    currentRating = rating;
    emit requestUpdate();
}

void RatingPainter::resizeEvent(QResizeEvent *event)
{
    size = event->size();
}
