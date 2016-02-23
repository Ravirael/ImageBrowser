#include "filelistwidgetitem.h"


FileListWidgetItem::FileListWidgetItem(const QIcon &icon, QFileInfo *file, QListWidget *parent) :
    QListWidgetItem(icon, file->baseName(), parent), file(file)
{

}

const QFileInfo *FileListWidgetItem::getFileInfo() const
{
    return file;
}
