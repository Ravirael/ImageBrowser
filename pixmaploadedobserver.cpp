#include "pixmaploadedobserver.h"

PixmapLoadedObserver::PixmapLoadedObserver(QObject *parent) : QObject(parent)
{

}

void PixmapLoadedObserver::operator()(const std::shared_ptr<AsyncPixmapLoader> &loader)
{
    emit loadingFinished(loader);
}
