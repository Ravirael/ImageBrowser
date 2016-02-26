#include "filelistwidgetitem.h"


FileListWidgetItem::FileListWidgetItem(const QIcon &icon, QFileInfo *file, QListWidget *parent) :
    QListWidgetItem(icon, file->baseName(), parent), file(file)
{
    firstPixmap = icon.pixmap(QSize(1024, 1024));
}

const QFileInfo *FileListWidgetItem::getFileInfo() const
{
    return file;
}

QPixmap FileListWidgetItem::getFirstPixmap() const
{
    return firstPixmap;
}
