#include "imagepainter.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QDir>

ImagePainter::ImagePainter(QWidget *parent) :
    QWidget(parent), pixmap(nullptr), scale(1.0f)
{
}

/**
 * @deprecated Just for tests purpose
 * @brief ImagePainter::openFile load to static QPixmap and sets it as active
 * @param fileName path to image file to open.
 * @return if opening has suceeded.
 */
bool ImagePainter::openFile(QString fileName)
{
    static QPixmap pixmap;

    pixmap.load(fileName);
    if (pixmap.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    reset();
    setPixmap(&pixmap);

    return true;
}

void ImagePainter::setPixmap(QPixmap *pixmap)
{
    bool calib = true;

    if (this->pixmap == pixmap)
    {
        calib = false;
    }

    this->pixmap = pixmap;

    if (calib)
    {
        calibrate();
    }

    update();
}

/**
 * @brief ImagePainter::reset resets all positons and sets scale to 1.
 */
void ImagePainter::reset()
{
    pixmap = nullptr;
    scale = 1.0f;
    position = positionOrigin = mouseOrigin = QPoint();
}

void ImagePainter::calibrate()
{
    const qreal scale_correction = 0.98;
    QPoint center(this->width()/2, this->height()/2);

    if (!pixmap)
        return;

    scale = 1.0f;

    if (pixmap->width()/(qreal)this->width() < pixmap->height()/(qreal)this->height())
    {
        scale *= this->height()*scale_correction/pixmap->height();
    }
    else
    {
        scale *= this->width()*scale_correction/pixmap->width();
    }

    //position = center;
    position = center - QPoint((scale*pixmap->width())/2, (scale*pixmap->height())/2);
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

    update();
}

void ImagePainter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(position);
    painter.scale(scale, scale);

    if (pixmap)
    {
        painter.drawPixmap(0, 0, *pixmap);
    }

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
        update();
    }
}

void ImagePainter::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit sizeChanged(event->size());
}
