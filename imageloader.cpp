#include "imageloader.h"
#include "iteratorhelper.h"

#include <chrono>
#include <QImageReader>

ImageLoader::ImageLoader(QObject *parent) :
    QObject(parent),
    supportedExtensions{"*.jpg","*.jpeg","*.png","*.bmp"},
    prevCount(4),
    postCount(8)
{
    currentPixmap = pixmaps.begin();
    currentFile = files.begin();
}

ImageLoader::~ImageLoader()
{

}

const std::vector<QFileInfo> &ImageLoader::getFiles() const
{
    return files;
}

void ImageLoader::next()
{
    IteratorHelper::circularIncrement(currentFile, files);

    if (IteratorHelper::circularIncrement(currentPixmap, pixmaps))
    {
        QString path = currentFile->filePath();

        pixmaps.emplace_back(AsyncPixmapLoader(path, size, this));
        connect(&(pixmaps.back()),
                SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                this,
                SLOT(newPixmap(AsyncPixmapLoader*)));
        pixmaps.back().load();
        currentPixmap = std::prev(pixmaps.end());
        reloadCurrent = true;
    }

    if (currentPixmap->isLoaded())
    {
        emit imageChanged(currentPixmap->getPixmap());
    }

    if (std::distance(pixmaps.begin(), currentPixmap) >= prevCount)
    {
        pixmaps.pop_front();
    }

    reloadCurrent = false;
    loadNext();
}

void ImageLoader::prev()
{
    IteratorHelper::circularDecrement(currentFile, files);

    if (IteratorHelper::circularDecrement(currentPixmap, pixmaps))
    {
        QString path = currentFile->filePath();

        pixmaps.emplace_front(AsyncPixmapLoader(path, size, this));
        connect(&(pixmaps.front()),
                SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                this,
                SLOT(newPixmap(AsyncPixmapLoader*)));
        pixmaps.front().load();
        currentPixmap = pixmaps.begin();
        reloadCurrent = true;
    }

    if (currentPixmap->isLoaded())
    {
        emit imageChanged(currentPixmap->getPixmap());
    }

    if (std::distance(currentPixmap, pixmaps.end()) > postCount)
    {
        pixmaps.pop_back();
    }

    reloadCurrent = false;
    loadNext();
}

void ImageLoader::setDir(QDir dir)
{
    QDirIterator it(dir.absolutePath(), supportedExtensions, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    emit imageChanged(nullptr);

    files.clear();
    pixmaps.clear();

    for (it.next(); it.hasNext(); it.next())
    {
        files.push_back(it.fileInfo());
    }

    currentPixmap = pixmaps.begin();
    currentFile = files.begin();

    loadNext();

    currentPixmap = pixmaps.begin();

}

void ImageLoader::setSize(QSize size)
{

    if (this->size.width() > size.width())
    {
        this->size = size;
        return;
    }

    this->size = size;

    if (pixmaps.empty())
    {
        return;
    }

    pixmaps.splice(pixmaps.begin(), pixmaps, currentPixmap);
    pixmaps.erase(std::next(pixmaps.begin()), pixmaps.end());
    currentPixmap = pixmaps.begin();

    currentPixmap->joinThread();
    currentPixmap->setSize(this->size);
    reloadCurrent = true;

    loadNext();
    currentPixmap = pixmaps.begin();
}

void ImageLoader::loadCurrentFullSize()
{
    reloadCurrent = !(currentPixmap->isFullSize());
    currentPixmap->setSize(QSize(0, 0));
    loadNext();
}

void ImageLoader::newPixmap(AsyncPixmapLoader *loader)
{
    if (loader == &(*currentPixmap))
    {
        emit imageChanged(currentPixmap->getPixmap());
    }

    loadNext();
}

void ImageLoader::loadNext()
{
    auto dist = std::distance(currentPixmap, pixmaps.end());

    if (files.empty())
    {
        return;
    }

    if ( !(pixmaps.empty()) &&
         (!(pixmaps.back().isLoaded()) ||
         !(pixmaps.front().isLoaded()) ||
          !(currentPixmap->isLoaded()) ))
    {
        return;
    }

    if (reloadCurrent)
    {
        currentPixmap->joinThread();
        currentPixmap->load();
        reloadCurrent = false;
        return;
    }

    if (dist <= postCount + 1)
    {
        QString path = IteratorHelper::circularNext(currentFile, files, dist)->filePath();

        pixmaps.emplace_back(AsyncPixmapLoader(path, size, this));
        connect(&(pixmaps.back()),
                SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                this,
                SLOT(newPixmap(AsyncPixmapLoader*)));
        pixmaps.back().load();
    }
    else
    {
        dist = std::distance(pixmaps.begin(), currentPixmap);

        if (dist <= prevCount)
        {
            QString path = IteratorHelper::circularNext(currentFile, files, -(dist + 1))->filePath();

            pixmaps.emplace_front(AsyncPixmapLoader(path, size, this));
            connect(&(pixmaps.front()),
                    SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                    this,
                    SLOT(newPixmap(AsyncPixmapLoader*)));
            pixmaps.front().load();
        }
    }
}

QPixmap ImageLoader::load(QString path)
{
    QPixmap pixmap;
    pixmap.load(path);
    return pixmap;
}

