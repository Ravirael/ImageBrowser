#include "ratingpainter.h"

#include <QImage>

RatingPainter::RatingPainter(QObject *parent):Drawable(parent),currentRating(RatingSystem::Medium)
{

    icons[RatingSystem::Highest] = ":)";

    icons[RatingSystem::Medium] = ":|";

    icons[RatingSystem::Lowest] = ":(";

}

RatingPainter::~RatingPainter()
{

}

void RatingPainter::draw(QWidget *canvas)
{
    QPainter painter(canvas);
    QFont font;
    font.setPixelSize(42);
    font.setBold(true);
    painter.setFont(font);
    painter.translate(32, 32);
    painter.rotate(90);
    painter.setPen(Qt::white);
    painter.drawText(2, -2, icons[currentRating]);
    painter.setPen(Qt::black);
    painter.drawText(0, 0, icons[currentRating]);

}

void RatingPainter::setRating(RatingSystem::Rating rating)
{
    currentRating = rating;
    emit requestUpdate();
}
