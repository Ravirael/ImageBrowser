#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QIcon>

#include "filelistwidgetitem.h"

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

    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*,QListWidgetItem*)));

    connect(&iconLoader, SIGNAL(iconLoaded(QIcon, QFileInfo*)), this, SLOT(iconLoaded(QIcon, QFileInfo*)));
    connect(painter, SIGNAL(sizeChanged(QSize)), &loader, SLOT(setSize(QSize)));
    connect(&loader, SIGNAL(itemChanged(int)), this, SLOT(itemChanged(int)));
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

    ui->listWidget->setCurrentRow(0);


//    dir=info.dir();
  //  fileList = dir.entryList();
 //   qDebug()<<dir.path();

    //window.setLayout(ui->horizontalLayout);
    //window.showFullScreen();
}

void MainWindow::iconLoaded(QIcon icon, QFileInfo *file)
{
    ui->listWidget->addItem(new FileListWidgetItem(icon, file));
    //ui->listWidget->repaint();
}

void MainWindow::itemChanged(QListWidgetItem *, QListWidgetItem *i)
{
    int row = ui->listWidget->currentRow();
    //qDebug() << "Slot activated. Row: " << ui->listWidget->currentRow();

    loader.selectFile(row);
    //loader.setFile(*(static_cast<FileListWidgetItem *>(i)->getFileInfo()));
}

void MainWindow::itemChanged(int index)
{
    ui->listWidget->blockSignals(true);
    ui->listWidget->setCurrentRow(index);
    ui->listWidget->blockSignals(false);
}


void MainWindow::on_actionNext_triggered()
{
//    ui->listWidget->blockSignals(true);
//    //ui->listWidget->setCurrentRow((ui->listWidget->currentRow()+1)%ui->listWidget->count());
//    ui->listWidget->blockSignals(false);
}


void MainWindow::on_actionPrev_triggered()
{
//    ui->listWidget->blockSignals(true);
//    int row = ui->listWidget->currentRow() - 1;
//    if (row < 0)
//    {
//        row = ui->listWidget->count() - 1;
//    }

//    row %= ui->listWidget->count();
//    //ui->listWidget->setCurrentRow(row);
//    ui->listWidget->blockSignals(false);
}
