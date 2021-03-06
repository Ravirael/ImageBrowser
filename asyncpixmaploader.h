#ifndef ASYNCPIXMAPLOADER_H
#define ASYNCPIXMAPLOADER_H
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <memory>
#include <atomic>

class AsyncPixmapLoader
{
    QSize maxSize;
    QString path;
    QImage image;
    QPixmap pixmap;
    std::atomic_bool ready;
    bool fullSize = false;
    bool reloaded;

public:
    explicit AsyncPixmapLoader(QString path, QSize maxSize = QSize(0, 0), QObject *parent = 0, bool reloaded = false);
    virtual ~AsyncPixmapLoader();
    bool isLoaded() const;
    QPixmap getPixmap();
    bool isFullSize() const;
    bool isReloaded() const;
    QSize getSize() const;

    void operator()();

public:
    void setSize(const QSize &size);
};
#endif //ASYNCPIXMAPLOADER_H

