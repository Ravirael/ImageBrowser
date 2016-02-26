#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QStandardPaths>

#include <type_traits>

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
    ui->canvas->addFollower(ratingPainter);

    //connect(painter, SIGNAL(update()), ui->canvas, SLOT(update()));
    connect(&loader, SIGNAL(imageChanged(std::shared_ptr<QPixmap>, bool)), painter, SLOT(setPixmap(std::shared_ptr<QPixmap>, bool)));

    connect(ui->actionNext, SIGNAL(triggered(bool)), &loader, SLOT(next()));
    connect(ui->actionPrev, SIGNAL(triggered(bool)), &loader, SLOT(prev()));

    //connect(ui->actionIncRating, SIGNAL(triggered(bool)), &ratingSystem, SLOT(incRating()));
    //connect(ui->actionDecRating, SIGNAL(triggered(bool)), &ratingSystem, SLOT(decRating()));


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


    QString formats = "Obrazy (*.jpg *.jpeg *.png *.bmp *.JPG *.JPEG *.PNG *.BMP);;Wszystkie (*.*)";
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last(),
                       formats);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);


    dialog.exec();

    openFile(dialog.selectedFiles().first());
}

void MainWindow::iconLoaded(QIcon icon, QFileInfo *file)
{
    ui->listWidget->addItem(new FileListWidgetItem(icon, file));
    drawPixmapOnIcon(ui->listWidget->count()-1);
}

void MainWindow::itemChanged(QListWidgetItem *, QListWidgetItem *i)
{
    int row = ui->listWidget->currentRow();
    loader.selectFile(row);
}

void MainWindow::itemChanged(int index)
{
    ui->listWidget->blockSignals(true);
    ui->listWidget->setCurrentRow(index);
    ui->listWidget->blockSignals(false);
    ui->listWidget->scrollToItem(ui->listWidget->currentItem(),
                                 QAbstractItemView::PositionAtCenter);

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

void MainWindow::drawPixmapOnIcon(int row)
{
    auto item = static_cast<FileListWidgetItem *>(ui->listWidget->item(row));
    QPixmap icon = item->getFirstPixmap();
    QPainter painter(&icon);
    painter.setOpacity(0.75);
    painter.drawPixmap(8, 8, 32, 32, ratingPainter.getPixmap(ratingSystem.ratingOf(row)));
    item->setIcon(QIcon(icon));
    //ui->listWidget->repaint();
}

void MainWindow::on_actionIncRating_triggered()
{
    ratingSystem.incRating();
    drawPixmapOnIcon(ui->listWidget->currentRow());
    ratingPainter.display();
}

void MainWindow::on_actionDecRating_triggered()
{
    ratingSystem.decRating();
    drawPixmapOnIcon(ui->listWidget->currentRow());
    ratingPainter.display();
}

void MainWindow::openFile(const QString &path, QDirIterator::IteratorFlag flags)
{
    QFileInfo file(path);
    int index = 0;

    if (file.isDir())
    {
        ratingSystem.setDir(QDir(path), flags);
    }
    else
    {
        ratingSystem.setDir(file.dir(), flags);
    }

    iconLoader.setFiles(ratingSystem.getFiles());
    loader.setFiles(ratingSystem.getFiles());

    if (file.isFile())
    {
        auto files = ratingSystem.getFiles();
        index = std::distance(files.begin(), std::find(files.begin(), files.end(), file));
        loader.selectFile(index);
    }


    ui->listWidget->blockSignals(true);
    ui->listWidget->clear();
    iconLoader.loadIconsAsync();

    ui->listWidget->setCurrentRow(index);
    ui->listWidget->blockSignals(false);
}
