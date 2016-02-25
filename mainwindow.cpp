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
    //ui->scrollArea->setWidget(painter);
    ui->canvas->addDrawable(*painter);
    ui->canvas->addFollower(*painter);
    ui->canvas->addDrawable(ratingPainter);

    //connect(painter, SIGNAL(update()), ui->canvas, SLOT(update()));
    connect(&loader, SIGNAL(imageChanged(std::shared_ptr<QPixmap>, bool)), painter, SLOT(setPixmap(std::shared_ptr<QPixmap>, bool)));

    connect(ui->actionNext, SIGNAL(triggered(bool)), &loader, SLOT(next()));
    connect(ui->actionPrev, SIGNAL(triggered(bool)), &loader, SLOT(prev()));

    connect(ui->actionIncRating, SIGNAL(triggered(bool)), &ratingSystem, SLOT(incRating()));
    connect(ui->actionDecRating, SIGNAL(triggered(bool)), &ratingSystem, SLOT(decRating()));


    connect(&ratingSystem, SIGNAL(ratingChanged(RatingSystem::Rating)), &ratingPainter, SLOT(setRating(RatingSystem::Rating)));


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

    ratingSystem.setDir(QDir(dirPath));

    iconLoader.setFiles(ratingSystem.getFiles());
    loader.setFiles(ratingSystem.getFiles());

    ui->listWidget->blockSignals(true);
    ui->listWidget->clear();

    iconLoader.loadIconsAsync();

    ui->listWidget->setCurrentRow(0);
    ui->listWidget->blockSignals(false);



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
    //ui->listWidget->scrollToItem(i, QAbstractItemView::PositionAtCenter	);

    loader.selectFile(row);
    //loader.setFile(*(static_cast<FileListWidgetItem *>(i)->getFileInfo()));
}

void MainWindow::itemChanged(int index)
{
    ui->listWidget->blockSignals(true);
    ui->listWidget->setCurrentRow(index);
    ui->listWidget->blockSignals(false);
    ui->listWidget->scrollToItem(ui->listWidget->currentItem(), QAbstractItemView::PositionAtCenter	);

    ratingSystem.setActiveFile(index);
}



void MainWindow::on_actionDeleteLow_triggered()
{
    iconLoader.stopLoading();

    loader.setFiles(ratingSystem.getFiles({RatingSystem::Medium, RatingSystem::Highest}));
    iconLoader.setFiles(ratingSystem.getFiles({RatingSystem::Medium, RatingSystem::Highest}));

    auto toDelete = ratingSystem.getFiles({RatingSystem::Lowest});

    for (auto &file : toDelete)
    {
        QFile::remove(file.filePath());
    }

    ui->listWidget->blockSignals(true);
    ui->listWidget->clear();

    iconLoader.loadIconsAsync();

    ui->listWidget->setCurrentRow(0);
    ui->listWidget->blockSignals(false);
}

void MainWindow::on_actionMoveGood_triggered()
{
    iconLoader.stopLoading();

    loader.setFiles(ratingSystem.getFiles({RatingSystem::Medium, RatingSystem::Lowest}));
    iconLoader.setFiles(ratingSystem.getFiles({RatingSystem::Medium, RatingSystem::Lowest}));

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                  "",
                 QFileDialog::ShowDirsOnly
                 | QFileDialog::DontResolveSymlinks);

    QDir dir(dirPath);

    auto toMove = ratingSystem.getFiles({RatingSystem::Highest});

    for (auto &file : toMove)
    {
        Q_ASSERT(QFile::rename(file.absoluteFilePath(), dir.absoluteFilePath(file.fileName())));
    }

    ui->listWidget->blockSignals(true);
    ui->listWidget->clear();

    iconLoader.loadIconsAsync();

    ui->listWidget->setCurrentRow(0);
    ui->listWidget->blockSignals(false);

}
