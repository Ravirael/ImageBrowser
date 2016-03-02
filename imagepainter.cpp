#include "imagepainter.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QDir>

ImagePainter::ImagePainter(QWidget *parent) :
    Drawable(parent), scale(1.0f)
{
}

void ImagePainter::setPixmap(QPixmap pixmap, bool reload)
{
    static QSize prevSize;
    bool calib = true;

    /*if (!pixmap)
    {
        this->pixmap = pixmap;
        emit requestUpdate();
        return;
    }*/

    if (reload)
    {
        double scaleRatio = (double)prevSize.height()/pixmap.size().height();
        scale *= scaleRatio;
        calib = false;
    }

    this->pixmap = pixmap;

    if (calib)
    {
        calibrate();
    }

    emit requestUpdate();

    prevSize = this->pixmap.size();
}

/**
 * @brief ImagePainter::reset resets all positons and sets scale to 1.
 */
void ImagePainter::reset()
{
    pixmap = QPixmap();
    scale = 1.0f;
    position = positionOrigin = mouseOrigin = QPoint();
}

void ImagePainter::calibrate()
{
    const qreal scale_correction = 0.98;
    QPoint center(size.width()/2, size.height()/2);

    if (!pixmap)
        return;

    scale = 1.0f;

    if (pixmap.width()/(qreal)size.width() < pixmap.height()/(qreal)size.height())
    {
        scale *= size.height()*scale_correction/pixmap.height();
    }
    else
    {
        scale *= size.width()*scale_correction/pixmap.width();
    }

    position = center - QPoint((scale*pixmap.width())/2, (scale*pixmap.height())/2);
}

void ImagePainter::draw(QWidget *canvas)
{
    QPainter painter(canvas);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(position);
    painter.scale(scale, scale);

    //if (!pixmap.isNull())
    //{
        painter.drawPixmap(0, 0, pixmap);
    //}
}

void ImagePainter::wheelEvent(QWheelEvent *event)
{
    double scaleFactor=1.1;

    if (event->delta() < 0)
    {
        scaleFactor=1.0/scaleFactor;
    }
    else
    {
        emit zoomed();
    }

    scale *= scaleFactor;
    position.rx() = scaleFactor*position.x() + (1.0-scaleFactor)*event->x();
    position.ry() = scaleFactor*position.y() + (1.0-scaleFactor)*event->y();
}

void ImagePainter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        mouseOrigin = event->pos();
        positionOrigin = position;
    }
}

void ImagePainter::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        position = positionOrigin - mouseOrigin + event->pos();
    }
}

void ImagePainter::resizeEvent(QResizeEvent *event)
{
    size = event->size();
    emit sizeChanged(event->size());
    calibrate();

}
