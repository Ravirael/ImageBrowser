#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QObject>
#include <QFileInfo>
#include <QImageReader>
#include <QIcon>

#include <thread>
#include <atomic>
#include <vector>

class IconLoader : public QObject
{
    Q_OBJECT

    QImageReader reader;
    std::vector<QFileInfo> files;
    decltype(files.begin()) currentFile;
    std::atomic_bool stop;
    std::thread loadingThread;


public:
    explicit IconLoader(QObject *parent = 0);
    ~IconLoader();


signals:
    void iconLoaded(QIcon icon, QFileInfo *file);

public slots:
    void setFiles(std::vector<QFileInfo> files);
    void loadIconsAsync();
    void loadIcons();
    void stopLoading();

};

#endif // ICONLOADER_H
