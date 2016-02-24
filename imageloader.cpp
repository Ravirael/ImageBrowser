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

    qRegisterMetaType<std::shared_ptr<AsyncPixmapLoader>>("std::shared_ptr<AsyncPixmapLoader>");
    connect(&observer, SIGNAL(loadingFinished(std::shared_ptr<AsyncPixmapLoader>)), this, SLOT(newPixmap(std::shared_ptr<AsyncPixmapLoader>)));

    loadingThread = std::thread([&]{loadingQueue.performTasks();});
}

ImageLoader::~ImageLoader()
{
    loadingQueue.stopTasks();
    loadingThread.join();
}

const std::vector<QFileInfo> &ImageLoader::getFiles() const
{
    return files;
}

void ImageLoader::next()
{
    IteratorHelper::circularIncrement(currentFile, files);

    emit itemChanged(std::distance(files.begin(), currentFile));

    IteratorHelper::circularIncrement(currentPixmap, pixmaps);

    if ((*currentPixmap)->isLoaded())
    {
        emit imageChanged((*currentPixmap)->getPixmap(), false);
    }

    QString path = IteratorHelper::circularNext(currentFile, files, postCount)->filePath();

    pixmaps.pop_front();
    pixmaps.emplace_back(std::make_shared<AsyncPixmapLoader>(path, size, this));
    updateQueue();
}

void ImageLoader::prev()
{
    IteratorHelper::circularDecrement(currentFile, files);

    emit itemChanged(std::distance(files.begin(), currentFile));

    IteratorHelper::circularDecrement(currentPixmap, pixmaps);

    if ((*currentPixmap)->isLoaded())
    {
        emit imageChanged((*currentPixmap)->getPixmap(), false);
    }

    QString path = IteratorHelper::circularNext(currentFile, files, -prevCount)->filePath();

    pixmaps.pop_back();
    pixmaps.emplace_front(std::make_shared<AsyncPixmapLoader>(path, size, this));
    updateQueue();
}

void ImageLoader::setDir(QDir dir)
{
    dir.setSorting(QDir::Name);

    QDirIterator it(dir.absolutePath(), supportedExtensions, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    files.clear();

    for (it.next(); it.hasNext(); it.next())
    {
        files.push_back(it.fileInfo());
    }

    currentFile = files.begin();

    fillQueue();


    emit itemChanged(0);

}


void ImageLoader::selectFile(int index)
{
    currentFile = std::next(files.begin(), index);
    fillQueue();
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

    fillQueue();
}

void ImageLoader::loadCurrentFullSize()
{
    if ((*currentPixmap)->isFullSize())
    {
        return;
    }

    auto inserted = pixmaps.insert(currentPixmap,
                                   std::make_shared<AsyncPixmapLoader>(currentFile->filePath(), QSize(0, 0), this, true));
    pixmaps.remove(*currentPixmap);
    currentPixmap = inserted;

    loadingQueue.addToBegining(*currentPixmap);
}

void ImageLoader::newPixmap(std::shared_ptr<AsyncPixmapLoader> loader)
{
    if (loader == *currentPixmap)
    {
        emit imageChanged((*currentPixmap)->getPixmap(), (*currentPixmap)->isReloaded());
    }
}

void ImageLoader::fillQueue()
{
    auto iter = IteratorHelper::circularNext(currentFile, files);

    pixmaps.clear();
    loadingQueue.clear();

    pixmaps.emplace_back(std::make_shared<AsyncPixmapLoader>(currentFile->filePath(), size, this));
    loadingQueue.addToBegining(pixmaps.back());

    currentPixmap = pixmaps.begin();

    for (unsigned i = 0; i < postCount; ++i)
    {
        QString path = iter->filePath();
        IteratorHelper::circularIncrement(iter, files);
        pixmaps.emplace_back(std::make_shared<AsyncPixmapLoader>(path, size, this));
        loadingQueue.addToEnd(pixmaps.back());
    }

    iter = IteratorHelper::circularNext(currentFile, files, -1);

    for (unsigned i = 0; i < prevCount; ++i)
    {
        QString path = iter->filePath();
        IteratorHelper::circularDecrement(iter, files);
        pixmaps.emplace_front(std::make_shared<AsyncPixmapLoader>(path, size, this));
        loadingQueue.addToEnd(pixmaps.front());
    }
}

void ImageLoader::updateQueue()
{
    loadingQueue.clear();

    for(auto iter = currentPixmap; iter != pixmaps.end(); ++iter)
    {
        if (!(*iter)->isLoaded())
        {
            loadingQueue.addToEnd(*iter);
        }
    }

    for(auto iter = pixmaps.begin(); iter != pixmaps.end(); ++iter)
    {
        if (!(*iter)->isLoaded())
        {
            loadingQueue.addToEnd(*iter);
        }
    }
}

