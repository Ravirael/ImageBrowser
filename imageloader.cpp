#include "imageloader.h"
#include "iteratorhelper.h"

#include <chrono>
#include <QImageReader>

ImageLoader::ImageLoader(QObject *parent) :
    QObject(parent),
    prevCount(4),
    postCount(8)
{
    setObjectName("Main ImageLoader");
    currentPixmap = pixmaps.begin();
    currentFile = files.begin();


    qRegisterMetaType<std::shared_ptr<AsyncPixmapLoader>>("std::shared_ptr<AsyncPixmapLoader>");
    connect(&loadingQueue, SIGNAL(loadingFinished(std::shared_ptr<AsyncPixmapLoader>)),
            this, SLOT(newPixmap(std::shared_ptr<AsyncPixmapLoader>)), Qt::QueuedConnection);

    loadingQueue.start();
    //loadingThread = std::thread(&ImageLoadingQueue::performTasks, &loadingQueue);
}

ImageLoader::~ImageLoader()
{
    loadingQueue.stopTasks();
    loadingQueue.quit();
    loadingQueue.wait();
    //loadingThread.join();
}

const std::vector<QFileInfo> &ImageLoader::getFiles() const
{
    return files;
}

int ImageLoader::currentIndex() const
{
    return std::distance<std::vector<QFileInfo>::const_iterator>(files.begin(), currentFile);
}

void ImageLoader::switchElement(int distance)
{
    IteratorHelper::circularAdvance(currentFile, files, distance);
    std::advance(currentPixmap, distance);
}

void ImageLoader::next()
{
    switchElement(1);

    QString path = IteratorHelper::circularNext(currentFile, files, postCount)->filePath();

    pixmaps.pop_front();
    pixmaps.emplace_back(std::make_shared<AsyncPixmapLoader>(path, size, this));
}

void ImageLoader::prev()
{
    switchElement(-1);

    QString path = IteratorHelper::circularNext(currentFile, files, -prevCount)->filePath();

    pixmaps.pop_back();
    pixmaps.emplace_front(std::make_shared<AsyncPixmapLoader>(path, size, this));
}


void ImageLoader::setFiles(std::vector<QFileInfo> newFiles)
{

    files = newFiles;
    currentFile = files.begin();

    fillQueue();

    emit itemChanged(0);
}


void ImageLoader::selectFile(int index)
{
    //TODO that's probably not the best way

    unsigned distance = std::abs(index - currentIndex());
    bool foreward = (index > currentIndex());

    if (distance > (files.size() - distance))
    {
        distance = files.size() - distance;
        foreward = !foreward;
    }

    for (unsigned i = 0; i < distance; ++i)
    {
        if (foreward)
        {
            next();
        }
        else
        {
            prev();
        }
    }
    updateQueue();

    if ((*currentPixmap)->isLoaded())
    {
        emit imageChanged((*currentPixmap)->getPixmap(), false);
    }

    emit itemChanged(std::distance(files.begin(), currentFile));

}

void ImageLoader::stopLoading()
{
    loadingQueue.clear();

    while (!loadingQueue.isWaiting())
    {
        std::this_thread::yield();
    }

    files.clear();
    pixmaps.clear();
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

    for(auto iter = pixmaps.begin(); iter != currentPixmap; ++iter)
    {
        if (!(*iter)->isLoaded())
        {
            loadingQueue.addToEnd(*iter);
        }
    }
}



