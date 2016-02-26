#ifndef RATINGPAINTER_H
#define RATINGPAINTER_H

#include <QPainter>
#include <QPixmap>
#include <QTimer>

#include <map>
#include "drawable.h"
#include "ratingsystem.h"
#include "eventdependanti.h"

class RatingPainter : public Drawable, public EventDependantI
{
    Q_OBJECT

    std::map<RatingSystem::Rating, QPixmap> icons;
    RatingSystem::Rating currentRating;
    QSize size;
    QTimer timer, fadeTimer;
    qreal opacity = 0.0;

public:
    RatingPainter(QObject *parent = 0);
    virtual ~RatingPainter();

    QPixmap getPixmap() const;
    QPixmap getPixmap(RatingSystem::Rating) const;

    // Drawable interface
public slots:
    void draw(QWidget *canvas) override;
    void display();
    void fade();
    void setRating(RatingSystem::Rating rating);


    // EventDependantI interface
public:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *) override{}
    void mouseMoveEvent(QMouseEvent *) override{}
    void wheelEvent(QWheelEvent *) override{}
};

#endif // RATINGPAINTER_H
