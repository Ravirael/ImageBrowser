#ifndef PIXMAPLOADEDOBSERVER_H
#define PIXMAPLOADEDOBSERVER_H

#include <QObject>
#include <memory>
#include "asyncpixmaploader.h"

class PixmapLoadedObserver : public QObject
{
    Q_OBJECT

public:
    explicit PixmapLoadedObserver(QObject *parent = 0);

    void operator()(const std::shared_ptr<AsyncPixmapLoader> &loader);

signals:
    void loadingFinished(std::shared_ptr<AsyncPixmapLoader> loader);

public slots:
};

#endif // PIXMAPLOADEDOBSERVER_H
