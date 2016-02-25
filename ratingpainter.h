#ifndef RATINGPAINTER_H
#define RATINGPAINTER_H

#include <QPainter>
#include <QPixmap>

#include <map>
#include "drawable.h"
#include "ratingsystem.h"

class RatingPainter : public Drawable
{
    Q_OBJECT

    std::map<RatingSystem::Rating, QString> icons;
    RatingSystem::Rating currentRating;

public:
    RatingPainter(QObject *parent = 0);
    virtual ~RatingPainter();

    // Drawable interface
public slots:
    void draw(QWidget *canvas) override;

    void setRating(RatingSystem::Rating rating);
};

#endif // RATINGPAINTER_H
