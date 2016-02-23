#ifndef FILELISTWIDGETITEM_H
#define FILELISTWIDGETITEM_H

#include <QWidget>
#include <QListWidgetItem>
#include <QListWidget>
#include <QIcon>
#include <QFileInfo>

class FileListWidgetItem : public QListWidgetItem
{
    //Q_OBJECT

    QFileInfo *file;

public:
    explicit FileListWidgetItem(const QIcon &icon, QFileInfo *file, QListWidget *parent = 0);

    const QFileInfo *getFileInfo() const;

//signals:

//public slots:
};

#endif // FILELISTWIDGETITEM_H
