#include "imageloadingqueue.h"

ImageLoadingQueue::ImageLoadingQueue(QObject *parent) :
    QThread(parent),
    LoadingQueue<std::shared_ptr<AsyncPixmapLoader>, ImageLoadingQueue&>(*this)
{

}

void ImageLoadingQueue::operator()(std::shared_ptr<AsyncPixmapLoader> loader)
{
    emit loadingFinished(loader);
}

ImageLoadingQueue::~ImageLoadingQueue()
{

}

