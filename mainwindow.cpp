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
#include "signalblocker.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->canvas->addDrawable(painter);
    ui->canvas->addFollower(painter);
    ui->canvas->addDrawable(ratingPainter);
    ui->canvas->addFollower(ratingPainter);

    connect(&loader, SIGNAL(imageChanged(QPixmap, bool)),
            &painter, SLOT(setPixmap(QPixmap, bool)));

    connect(ui->actionNext, SIGNAL(triggered(bool)),
            &ratingSystem, SLOT(next()));

    connect(ui->actionPrev, SIGNAL(triggered(bool)),
            &ratingSystem, SLOT(prev()));

    connect(&ratingSystem, SIGNAL(ratingChanged(RatingSystem::Rating)),
            &ratingPainter, SLOT(setRating(RatingSystem::Rating)));

    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(200,200));
    ui->listWidget->setResizeMode(QListWidget::Adjust);
    ui->listWidget->setFlow(QListWidget::LeftToRight);
    ui->listWidget->setWrapping(false);
    ui->listWidget->setMovement(QListWidget::Static);

    connect(ui->listWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(listWidgetItemChanged()));

    connect(&iconLoader, SIGNAL(iconLoaded(QIcon, QFileInfo*)), this, SLOT(iconLoaded(QIcon, QFileInfo*)));
    connect(&painter, SIGNAL(sizeChanged(QSize)), &loader, SLOT(setSize(QSize)));
    connect(&ratingSystem, SIGNAL(fileChanged(int)), this, SLOT(itemChanged(int)));
    connect(&ratingSystem, SIGNAL(fileChanged(int)), &loader, SLOT(selectFile(int)));
    connect(&ratingSystem, SIGNAL(filesChanged(std::vector<QFileInfo>)),
            &loader, SLOT(setFiles(std::vector<QFileInfo>)));
    connect(&ratingSystem, SIGNAL(filesChanged(std::vector<QFileInfo>)),
            &iconLoader, SLOT(setFiles(std::vector<QFileInfo>)));



    connect(&painter, SIGNAL(zoomed()), &loader, SLOT(loadCurrentFullSize()));
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

    if (dialog.selectedFiles().empty())
    {
        return;
    }

    openFile(dialog.selectedFiles().first());
}

void MainWindow::iconLoaded(QIcon icon, QFileInfo *file)
{
    auto item = new FileListWidgetItem(icon, file);
    ui->listWidget->addItem(item);
    drawPixmapOnIcon(ui->listWidget->count()-1);

    if (ui->listWidget->row(item) == loader.currentIndex())
    {
        itemChanged(loader.currentIndex());
    }

    QString message = QString("Ładowanie miniatur: %1/%2").
            arg(ui->listWidget->count()).
            arg(ratingSystem.getFiles().size());

    if (ui->listWidget->count() < ratingSystem.getFiles().size())
        ui->statusBar->showMessage(message);
    else
        ui->statusBar->showMessage("Ładowanie miniatur zakończone!", 2000);

}

void MainWindow::listWidgetItemChanged()
{
    int row = ui->listWidget->currentRow();
    ratingSystem.setActiveFile(row);
}


void MainWindow::itemChanged(int index)
{
    SignalBlocker blocker(ui->listWidget);
    ui->listWidget->setCurrentRow(index);
    ui->listWidget->scrollToItem(ui->listWidget->currentItem(),
                                 QAbstractItemView::PositionAtCenter);

}



void MainWindow::on_actionDeleteLow_triggered()
{
    iconLoader.stopLoading();

    auto toDelete = ratingSystem.popFiles({RatingSystem::Lowest});

    for (auto &file : toDelete)
    {
        QFile::remove(file.filePath());
    }

    ui->listWidget->clear();

    iconLoader.loadIconsAsync();
}

void MainWindow::on_actionMoveGood_triggered()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                  "",
                 QFileDialog::ShowDirsOnly);

    QDir dir(dirPath);

    loader.stopLoading();

    auto toMove = ratingSystem.popFiles({RatingSystem::Highest});

    for (auto &file : toMove)
    {
        QFile f(file.absoluteFilePath());

        Q_ASSERT(f.rename(dir.absoluteFilePath(file.fileName())));
    }

    ui->listWidget->clear();
    iconLoader.loadIconsAsync();
}

void MainWindow::drawPixmapOnIcon(int row)
{
    auto item = static_cast<FileListWidgetItem *>(ui->listWidget->item(row));

    if (!item)
    {
        return;
    }

    QPixmap icon = item->getFirstPixmap();
    QPainter painter(&icon);
    painter.setOpacity(0.75);
    painter.drawPixmap(8, 8, 32, 32, ratingPainter.getPixmap(ratingSystem.ratingOf(row)));
    item->setIcon(QIcon(icon));
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

    ui->listWidget->clear();

    if (file.isDir())
    {
        ratingSystem.setDir(QDir(path), flags);
    }
    else
    {
        ratingSystem.setDir(file.dir(), flags);
    }

    //iconLoader.setFiles(ratingSystem.getFiles());
    //loader.setFiles(ratingSystem.getFiles());

    if (file.isFile())
    {
        auto files = ratingSystem.getFiles();
        int index = std::distance(files.begin(), std::find(files.begin(), files.end(), file));
        ratingSystem.setActiveFile(index);
    }

    iconLoader.loadIconsAsync();
}

void MainWindow::on_actionFullSize_triggered()
{
    static bool isFullSize = false;
    static int left, right, top, bottom, spacing;
    auto centralLayout = ui->centralWidget->layout();


    if (!isFullSize)
    {
        centralLayout->getContentsMargins(&left, &top,
                                          &right, &bottom);
        spacing = centralLayout->spacing();
        centralLayout->setContentsMargins(QMargins());
        centralLayout->setSpacing(0);
        ui->listWidget->setMaximumHeight(0);
        ui->menuBar->setMaximumHeight(0);
        this->showFullScreen();
        statusBar()->hide();
    }
    else
    {
        this->showNormal();
        centralLayout->setContentsMargins(left, top,
                                          right, bottom);
        centralLayout->setSpacing(spacing);
        ui->listWidget->setMaximumHeight(132);
        ui->menuBar->setMaximumHeight(10000);
        statusBar()->show();
    }

    isFullSize = !isFullSize;
}
