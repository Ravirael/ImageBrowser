#pragma once
/** std includes **/
#include <memory>
#include <vector>
#include <list>
#include <future>

/** Qt includes **/
#include <QDir>
#include <QDirIterator>
#include <QObject>
#include <QFileInfo>
#include <QPixmap>

#include <QThread>

//#include <thread>

#include "asyncpixmaploader.h"
#include "imageloadingqueue.h"
#include "pixmaploadedobserver.h"

class ImageLoader : public QObject
{
    Q_OBJECT

private:
    std::vector<QFileInfo> files;
    decltype(files.begin()) currentFile;
    unsigned prevCount, postCount;

    bool reloadCurrent = false;
    std::list<std::shared_ptr<AsyncPixmapLoader>> pixmaps;
    decltype(pixmaps.begin()) currentPixmap;
    QPixmap pixmap;
    QSize size;

    //PixmapLoadedObserver observer;
    //LoadingQueue<std::shared_ptr<AsyncPixmapLoader>, PixmapLoadedObserver&>
    ImageLoadingQueue loadingQueue;
    std::thread loadingThread;

public:
    explicit ImageLoader(QObject *parent = 0);
    virtual ~ImageLoader();

    const std::vector<QFileInfo> &getFiles() const;
    int currentIndex() const;

signals:
    void imageChanged(QPixmap pixmap, bool);
    void itemChanged(int index);

public slots:
    void setFiles(std::vector<QFileInfo> files);
    void setSize(QSize size);
    void loadCurrentFullSize();
    void selectFile(int index);
    void stopLoading();

private slots:
    void newPixmap(std::shared_ptr<AsyncPixmapLoader> pixmap);
    void fillQueue();
    void updateQueue();

    void next();
    void prev();
    void switchElement(int distance);

private:
    static QPixmap load(QString path);


};
