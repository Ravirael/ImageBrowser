//#include "loadingqueue.h"

//LoadingQueue::LoadingQueue(QObject *parent) : QObject(parent)
//{

//}

//void LoadingQueue::pixmapLoaded(QPixmap *pixmap)
//{
//    loaderQueue.pop();

//}

//void LoadingQueue::add(const AsyncPixmapLoader &loader)
//{
//    if (!loader.isLoaded())
//    {
//        loaderQueue.push(loader);
//        connect(&(loaderQueue.back()), SIGNAL(pixmapReady(QPixmap *)), this, SLOT(pixmapLoaded(QPixmap*)));
//    }
//}
