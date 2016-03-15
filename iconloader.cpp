#include "iconloader.h"

#include <QPixmap>
#include <QIcon>

void IconLoader::loadIcons()
{
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
}

IconLoader::IconLoader(QObject *parent) : QObject(parent), stop(true)
{
    reader.setQuality(0);
}

IconLoader::~IconLoader()
{
    stopLoading();
}

void IconLoader::setFiles(const std::vector<QFileInfo> &files)
{
    stopLoading();
    this->files = files;
    currentFile = this->files.begin();
    stop = false;
}



