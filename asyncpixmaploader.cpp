#include "asyncpixmaploader.h"
#include <QPixmap>
#include <QImageReader>

AsyncPixmapLoader::AsyncPixmapLoader(QString path, QSize maxSize, QObject *parent, bool reloaded) :
    maxSize(maxSize), path(path), ready(false), reloaded(reloaded)
{

}

AsyncPixmapLoader::~AsyncPixmapLoader()
{
}

bool AsyncPixmapLoader::isLoaded() const
{
    return ready;
}

QPixmap AsyncPixmapLoader::getPixmap()
{
    if (ready)
    {
        if (pixmap.isNull())
        {
            pixmap = QPixmap::fromImage(std::move(image));
        }
    }

    return pixmap;
}


bool AsyncPixmapLoader::isReloaded() const
{
    return reloaded;
}

bool AsyncPixmapLoader::isFullSize() const
{
    return (isLoaded() && fullSize);
}

QSize AsyncPixmapLoader::getSize() const
{
    return maxSize;
}

void AsyncPixmapLoader::operator()()
{
    QImageReader reader(path);

    fullSize = true;

    if (!maxSize.isNull())
    {
        QSize size = reader.size();
        qreal factor;

        if (size.width()/(qreal)maxSize.width() < size.height()/(qreal)maxSize.height())
        {
            factor = maxSize.height()/(qreal)size.height();
        }
        else
        {
            factor = maxSize.width()/(qreal)size.width();
        }

        if (factor < 1.0)
        {
            reader.setScaledSize(size*factor);
            fullSize = false;
        }
    }

    image = reader.read();
    ready = true;
}

void AsyncPixmapLoader::setSize(const QSize &size)
{
    maxSize = size;
}
