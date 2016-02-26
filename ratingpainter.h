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
    QTimer timer;
    qreal opacity = 1.0;

public:
    RatingPainter(QObject *parent = 0);
    virtual ~RatingPainter();

    // Drawable interface
public slots:
    void draw(QWidget *canvas) override;
    void drawMinature(QIcon *icon);
    void fade();
    void setRating(RatingSystem::Rating rating);

    // EventDependantI interface
public:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override{};
    void mouseMoveEvent(QMouseEvent *event) override{};
    void wheelEvent(QWheelEvent *event) override{};
};

#endif // RATINGPAINTER_H
