#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    painter = new ImagePainter;
    ui->scrollArea->setWidget(painter);

    connect(&loader, SIGNAL(imageChanged(QPixmap*)), painter, SLOT(setPixmap(QPixmap*)));

    connect(ui->actionNext, SIGNAL(triggered(bool)), &loader, SLOT(next()));
    connect(ui->actionPrev, SIGNAL(triggered(bool)), &loader, SLOT(prev()));

    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(200,200));
    ui->listWidget->setResizeMode(QListWidget::Adjust);
    ui->listWidget->setFlow(QListWidget::LeftToRight);
    ui->listWidget->setWrapping(false);
    connect(&iconLoader, SIGNAL(iconLoaded(QIcon, QString)), this, SLOT(iconLoaded(QIcon, QString)));
    connect(painter, SIGNAL(sizeChanged(QSize)), &loader, SLOT(setSize(QSize)));
    //connect(painter, SIGNAL(sizeChanged(QSize)), painter, SLOT(calibrate()));

    connect(painter, SIGNAL(zoomed()), &loader, SLOT(loadCurrentFullSize()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOtw_rz_triggered()
{
    iconLoader.stopLoading();
    //QString path = QFileDialog::getOpenFileName(this, tr("Otworz plik"), tr(""), tr("Plik JPG (*.jpg)"));
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                  "",
                 QFileDialog::ShowDirsOnly
                 | QFileDialog::DontResolveSymlinks);
    //QFileInfo info(path);

    loader.setDir(QDir(dirPath));

    iconLoader.setFiles(loader.getFiles());

    ui->listWidget->clear();

    iconLoader.loadIconsAsync();


//    dir=info.dir();
  //  fileList = dir.entryList();
 //   qDebug()<<dir.path();

    //window.setLayout(ui->horizontalLayout);
    //window.showFullScreen();
}

void MainWindow::iconLoaded(QIcon icon, QString name)
{
    ui->listWidget->addItem(new QListWidgetItem(icon, name));
    //ui->listWidget->repaint();
}

