#ifndef ASYNCPIXMAPLOADER_H
#define ASYNCPIXMAPLOADER_H
#include <QObject>
#include <QPixmap>
#include <memory>
#include <atomic>

class AsyncPixmapLoader : public QObject
{
    Q_OBJECT

    QString path;
    std::shared_ptr<QPixmap> pixmap;
    QSize maxSize;
    std::atomic_bool ready;
    bool fullSize = false;
    bool reloaded;

public:
    explicit AsyncPixmapLoader(QString path, QSize maxSize = QSize(0, 0), QObject *parent = 0, bool reloaded = false);
    AsyncPixmapLoader(const AsyncPixmapLoader &loader);
    virtual ~AsyncPixmapLoader();
    bool isLoaded() const;
    std::shared_ptr<QPixmap> getPixmap();
    bool isFullSize() const;
    bool isReloaded() const;
    QSize getSize() const;

    void operator()();

signals:
    void pixmapReady(std::shared_ptr<QPixmap>);
    void loadingFinished(AsyncPixmapLoader *);

public slots:
    void setSize(const QSize &size);
};
#endif //ASYNCPIXMAPLOADER_H

