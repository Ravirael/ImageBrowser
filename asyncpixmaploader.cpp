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
    {
        thread.join();
    }
}

bool AsyncPixmapLoader::isFullSize()
{
    return (isLoaded() && fullSize);
}


void AsyncPixmapLoader::load()
{
    auto loadOp = [&]
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

        pixmap = QPixmap::fromImage(reader.read());
        ready = true;
        emit loadingFinished(this);
        emit pixmapReady(&pixmap);
    };

    ready = false;
    thread = std::thread(loadOp);
}

void AsyncPixmapLoader::setSize(const QSize &size)
{
    maxSize = size;
}
