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
    void iconLoaded(QIcon icon, QString name);


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
