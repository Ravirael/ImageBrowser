#include "iconloader.h"

#include <QPixmap>
#include <QIcon>

void IconLoader::loadIcons()
{
    QImageReader reader;
    reader.setQuality(0);

    for (auto currentFile = files.begin();
         (currentFile != files.end()) && (stop == false);
         ++currentFile)
    {
        reader.setFileName(currentFile->absoluteFilePath());
        QSize size = reader.size();
        double factor = 100.0/size.height();

        size *= factor;

        reader.setScaledSize(size);
        QPixmap pixmap = QPixmap::fromImageReader(&reader);
        emit iconLoaded(QIcon(pixmap), &(*currentFile));
    }
}

void IconLoader::loadIconsAsync()
{
    loadingThread = std::thread(&IconLoader::loadIcons, this);
}

void IconLoader::stopLoading()
{
    stop = true;
    if (loadingThread.joinable())
    {
        loadingThread.join();
    }

    files.clear();
}

IconLoader::IconLoader(QObject *parent) : QObject(parent), stop(true)
{
}

IconLoader::~IconLoader()
{
    stopLoading();
}

void IconLoader::setFiles(std::vector<QFileInfo> files)
{
    stopLoading();
    this->files = files;
    currentFile = this->files.begin();
    stop = false;
}



