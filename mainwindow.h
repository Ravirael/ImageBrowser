#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDir>

#include <thread>
#include <memory>

#include "imagepainter.h"
#include "imageloader.h"
#include "iconloader.h"

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOtw_rz_triggered();
    void iconLoaded(QIcon icon, QFileInfo *file);
    void itemChanged(QListWidgetItem *, QListWidgetItem *);
    void itemChanged(int index);

    void on_actionNext_triggered();

    void on_actionPrev_triggered();

private:
    ImageLoader loader;
    ImagePainter *painter;
    IconLoader iconLoader;
    QWidget window;
    Ui::MainWindow *ui;
    QDir dir;
    QStringList fileList;

protected:
};

#endif // MAINWINDOW_H
