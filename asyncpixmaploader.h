#pragma once

#include <QObject>
#include <QPixmap>
#include <future>
#include <thread>
#include <memory>
#include <atomic>

class AsyncPixmapLoader : public QObject
{
    Q_OBJECT

    QString path;
    QPixmap pixmap;
    QSize maxSize;
    std::atomic_bool ready;
    std::thread thread;

public:
    explicit AsyncPixmapLoader(QString path, QSize maxSize = QSize(0, 0), QObject *parent = 0);
    AsyncPixmapLoader(const AsyncPixmapLoader &loader);
    ~AsyncPixmapLoader();
    bool isLoaded() const;
    QPixmap *getPixmap();
    void joinThread();
    bool isFullSize();

signals:
    void pixmapReady(QPixmap *);
    void loadingFinished(AsyncPixmapLoader *);

public slots:
    void load();
    void setSize(const QSize &size);
};

