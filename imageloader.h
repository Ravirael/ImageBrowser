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

#include "asyncpixmaploader.h"
#include "loadingqueue.h"
#include "pixmaploadedobserver.h"

class ImageLoader : public QObject
{
    Q_OBJECT

private:
    const QStringList supportedExtensions;
    std::vector<QFileInfo> files;
    decltype(files.begin()) currentFile;
    unsigned prevCount, postCount;

    bool reloadCurrent = false;
    std::list<AsyncPixmapLoader> pixmaps;
    decltype(pixmaps.begin()) currentPixmap;
    QPixmap pixmap;
    QSize size;

    PixmapLoadedObserver observer;
    LoadingQueue<AsyncPixmapLoader, PixmapLoadedObserver> loadingQueue;

public:
    explicit ImageLoader(QObject *parent = 0);
    ~ImageLoader();

    const std::vector<QFileInfo> &getFiles() const;

signals:
    void imageChanged(QPixmap *pixmap);
    void itemChanged(int index);

public slots:
    void next();
    void prev();
    void setDir(QDir dir);
    void setSize(QSize size);
    void loadCurrentFullSize();
    bool setFile(QFileInfo file);
    void selectFile(int index);


private slots:
    void newPixmap(AsyncPixmapLoader *pixmap);
    void loadNext();

private:
    static QPixmap load(QString path);


};
