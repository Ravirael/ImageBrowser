#include "asyncpixmaploader.h"
#include <QPixmap>
#include <QImageReader>
#include <chrono>

AsyncPixmapLoader::AsyncPixmapLoader(QString path, QSize maxSize, QObject *parent) :
    QObject(parent), ready(false), maxSize(maxSize), path(path)
{

}

AsyncPixmapLoader::AsyncPixmapLoader(const AsyncPixmapLoader &loader) :
    QObject(loader.parent()), ready(false), maxSize(loader.maxSize), path(loader.path)

{

}

AsyncPixmapLoader::~AsyncPixmapLoader()
{
    joinThread();
}

bool AsyncPixmapLoader::isLoaded() const
{
    return ready;
}

QPixmap *AsyncPixmapLoader::getPixmap()
{
    if (ready)
    {
        return &pixmap;
    }

    return nullptr;
}

void AsyncPixmapLoader::joinThread()
{
    if (thread.joinable())
        thread.join();
}

bool AsyncPixmapLoader::isFullSize()
{
    return maxSize.isNull();
}


void AsyncPixmapLoader::load()
{
    auto loadOp = [&]
    {
        QImageReader reader(path);

        if (!maxSize.isNull())
        {
            QSize size = reader.size();

            if (size.width()/(qreal)maxSize.width() < size.height()/(qreal)maxSize.height())
            {
                size *= maxSize.height()/(qreal)size.height();
            }
            else
            {
                size *= maxSize.width()/(qreal)size.width();
            }

            reader.setScaledSize(size);
        }

        pixmap = QPixmap::fromImage(reader.read());
        ready = true;
        emit loadingFinished(this);
        emit pixmapReady(&pixmap);
    };

    thread = std::thread(loadOp);
}

void AsyncPixmapLoader::setSize(const QSize &size)
{
    maxSize = size;
}
