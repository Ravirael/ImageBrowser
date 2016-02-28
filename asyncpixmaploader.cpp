#include "asyncpixmaploader.h"
#include <QPixmap>
#include <QImageReader>
#include <chrono>

AsyncPixmapLoader::AsyncPixmapLoader(QString path, QSize maxSize, QObject *parent, bool reloaded) :
    QObject(parent), ready(false), maxSize(maxSize), path(path), reloaded(reloaded)
{
    pixmap = std::make_shared<QPixmap>();
}

AsyncPixmapLoader::AsyncPixmapLoader(const AsyncPixmapLoader &loader) :
    QObject(loader.parent()), ready(false), maxSize(loader.maxSize), path(loader.path), reloaded(loader.reloaded)

{
    pixmap = loader.pixmap;
}

AsyncPixmapLoader::~AsyncPixmapLoader()
{
}

bool AsyncPixmapLoader::isLoaded() const
{
    return ready;
}

std::shared_ptr<QPixmap> AsyncPixmapLoader::getPixmap()
{
    if (ready)
    {
        return pixmap;
    }

    return nullptr;
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

    *pixmap = QPixmap::fromImage(reader.read());
    ready = true;
    emit loadingFinished(this);
    emit pixmapReady(pixmap);

}


void AsyncPixmapLoader::setSize(const QSize &size)
{
    maxSize = size;
}
