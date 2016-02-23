#include "imageloader.h"
#include "iteratorhelper.h"

#include <chrono>
#include <QImageReader>

ImageLoader::ImageLoader(QObject *parent) :
    QObject(parent),
    supportedExtensions{"*.jpg","*.jpeg","*.png","*.bmp"},
    prevCount(4),
    postCount(8),
    loadingQueue(observer)
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
    if (!currentPixmap->isLoaded())
    {
        reloadCurrent = true;

        return;
    }

    IteratorHelper::circularIncrement(currentFile, files);

    emit itemChanged(std::distance(files.begin(), currentFile));

    reloadCurrent = false;

    if (IteratorHelper::circularIncrement(currentPixmap, pixmaps))
    {
        QString path = currentFile->filePath();

        loadingQueue.addToBegining(std::make_shared<AsyncPixmapLoader>(path, size, this));

        pixmaps.emplace_back(AsyncPixmapLoader(path, size, this));
        connect(&(pixmaps.back()),
                SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                this,
                SLOT(newPixmap(AsyncPixmapLoader*)));
        currentPixmap = std::prev(pixmaps.end());
        reloadCurrent = true;
    }
    else if (currentPixmap->isLoaded())
    {
        emit imageChanged(currentPixmap->getPixmap());
    }

    if (std::distance(pixmaps.begin(), currentPixmap) >= prevCount)
    {
        pixmaps.pop_front();
    }

    loadNext();
}

void ImageLoader::prev()
{
    if (!currentPixmap->isLoaded())
    {
        reloadCurrent = true;
        return;
    }

    IteratorHelper::circularDecrement(currentFile, files);

    emit itemChanged(std::distance(files.begin(), currentFile));

    reloadCurrent = false;

    if (IteratorHelper::circularDecrement(currentPixmap, pixmaps))
    {
        QString path = currentFile->filePath();

        pixmaps.emplace_front(AsyncPixmapLoader(path, size, this));

        connect(&(pixmaps.front()),
                SIGNAL(loadingFinished(AsyncPixmapLoader *)),
                this,
                SLOT(newPixmap(AsyncPixmapLoader*)));

        currentPixmap = pixmaps.begin();
        reloadCurrent = true;
    }
    else if (currentPixmap->isLoaded())
    {
        emit imageChanged(currentPixmap->getPixmap());
    }

    if (std::distance(currentPixmap, pixmaps.end()) > postCount)
    {
        pixmaps.pop_back();
    }

    loadNext();
}

void ImageLoader::setDir(QDir dir)
{
    dir.setSorting(QDir::Name);

    QDirIterator it(dir.absolutePath(), supportedExtensions, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    emit imageChanged(nullptr);


    files.clear();
    pixmaps.clear();

    for (it.next(); it.hasNext(); it.next())
    {
        files.push_back(it.fileInfo());
    }

    /*std::sort(files.begin(), files.end(),
              [](const QFileInfo &file1, const QFileInfo &file2) -> bool
    {
        return (file1.baseName() < file2.baseName());
    });*/


    currentPixmap = pixmaps.begin();
    currentFile = files.begin();

    loadNext();

    currentPixmap = pixmaps.begin();

    emit itemChanged(0);

}

bool ImageLoader::setFile(QFileInfo file)
{
    currentFile = std::find(files.begin(), files.end(), file);

    emit imageChanged(nullptr);
    pixmaps.clear();
    currentPixmap = pixmaps.begin();
    loadNext();
    currentPixmap = pixmaps.begin();
}

void ImageLoader::selectFile(int index)
{
    currentFile = std::next(files.begin(), index);

    emit imageChanged(nullptr);
    pixmaps.clear();
    currentPixmap = pixmaps.begin();
    loadNext();
    currentPixmap = pixmaps.begin();
}

void ImageLoader::setSize(QSize size)
{
    auto oldWidth = this->size.width();

    this->size = size;

    if ((oldWidth > size.width()) ||
        pixmaps.empty())
    {
        return;
    }

    pixmaps.splice(pixmaps.begin(), pixmaps, currentPixmap);
    pixmaps.erase(std::next(pixmaps.begin()), pixmaps.end());
    currentPixmap = pixmaps.begin();

    if (!currentPixmap->isFullSize() ||
         (this->size.width() > currentPixmap->getSize().width()))
    {
        currentPixmap->joinThread();
        currentPixmap->setSize(this->size);
        reloadCurrent = true;
    }

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

