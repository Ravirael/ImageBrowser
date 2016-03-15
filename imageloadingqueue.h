#ifndef IMAGELOADINGQUEUE_H
#define IMAGELOADINGQUEUE_H

#include <QThread>
#include <loadingqueue.h>
#include <asyncpixmaploader.h>

class ImageLoadingQueue : public QThread, public LoadingQueue<std::shared_ptr<AsyncPixmapLoader>, ImageLoadingQueue&>
{
    Q_OBJECT
public:
    explicit ImageLoadingQueue(QObject *parent = 0);
    void operator()(std::shared_ptr<AsyncPixmapLoader> loader);
    virtual ~ImageLoadingQueue();

signals:
    void loadingFinished(std::shared_ptr<AsyncPixmapLoader>);

public slots:

private slots:
    virtual void run() override
    {
        performTasks();
    }
};

#endif // IMAGELOADINGQUEUE_H
