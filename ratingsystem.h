#ifndef RATINGSYSTEM_H
#define RATINGSYSTEM_H

#include <QObject>
#include <QDir>
#include <QDirIterator>
#include <list>
#include <vector>
#include <utility>
#include <set>

class RatingSystem : public QObject
{
    Q_OBJECT

public:

    enum Rating
    {
        Lowest = 0, Medium, Highest
    };

private:

    std::vector<std::pair<QFileInfo, Rating>> files;
    decltype(files.begin()) currentFile;

public:
    explicit RatingSystem(QObject *parent = 0);

    std::vector<QFileInfo> getFiles() const;
    std::vector<QFileInfo> getFiles(const std::set<Rating> &desiredRating) const;
    std::vector<QFileInfo> popFiles(const std::set<Rating> &desiredRating);

    Rating ratingOf(int index) const;

signals:
    void ratingChanged(RatingSystem::Rating);
    void fileChanged(int index);
    void filesChanged(std::vector<QFileInfo> files);

public slots:
    void setDir(QDir dir, QDirIterator::IteratorFlag flags = QDirIterator::NoIteratorFlags);
    void setActiveFile(int index);
    void next();
    void prev();
    void incRating();
    void decRating();

private slots:
    void emitChangeSignals();
};

#endif // RATINGSYSTEM_H
