#ifndef PIXMAPLOADEDOBSERVER_H
#define PIXMAPLOADEDOBSERVER_H

#include <QObject>

class PixmapLoadedObserver : public QObject
{
    Q_OBJECT
public:
    explicit PixmapLoadedObserver(QObject *parent = 0);

signals:

public slots:
};

#endif // PIXMAPLOADEDOBSERVER_H