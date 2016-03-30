#include "ratingsystem.h"
#include "iteratorhelper.h"

RatingSystem::RatingSystem(QObject *parent) : QObject(parent)
{

}

std::vector<QFileInfo> RatingSystem::getFiles() const
{
    return getFiles({Lowest, Medium, Highest});
}

std::vector<QFileInfo> RatingSystem::getFiles(const std::set<RatingSystem::Rating> &desiredRating) const
{
    std::vector<QFileInfo> onlyFiles;

    for (auto &file : files)
    {
        if (desiredRating.count(file.second) != 0)
            onlyFiles.push_back(file.first);
    }

    return onlyFiles;
}

std::vector<QFileInfo> RatingSystem::popFiles(const std::set<RatingSystem::Rating> &desiredRating)
{

    auto removed = getFiles(desiredRating);

    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if (desiredRating.count(it->second) != 0)
        {
            it = files.erase(it);

            if (it != files.begin())
            {
                --it;
            }
        }
    }

    currentFile = files.begin();
    emit filesChanged(getFiles());
    emitChangeSignals();
    return removed;
}

RatingSystem::Rating RatingSystem::ratingOf(int index) const
{
    return std::next(files.begin(), index)->second;
}

void RatingSystem::setDir(QDir dir, QDirIterator::IteratorFlag flags)
{
    QDirIterator it(dir.absolutePath(),
                    QStringList{"*.jpg","*.jpeg","*.png","*.bmp"},
                    QDir::Files | QDir::NoDotAndDotDot,
                    flags);

    files.clear();

    for (it.next(); it.hasNext(); it.next())
    {
        files.push_back({it.fileInfo(), Rating::Medium});
    }

    files.push_back({it.fileInfo(), Rating::Medium});


    std::sort(files.begin(), files.end(),
              [](const auto &f1, const auto &f2) -> bool {return f1.first.fileName() < f2.first.fileName();});

    emit filesChanged(getFiles());
    setActiveFile(0);
}

void RatingSystem::setActiveFile(int index)
{
    currentFile = std::next(files.begin(), index);
    emitChangeSignals();
}

void RatingSystem::next()
{
    IteratorHelper::circularIncrement(currentFile, files);
    emitChangeSignals();
}

void RatingSystem::prev()
{
    IteratorHelper::circularDecrement(currentFile, files);
    emitChangeSignals();
}

void RatingSystem::incRating()
{
    if (currentFile->second < Highest)
    {
        int i = (int)currentFile->second;
        currentFile->second = (Rating)(++i);
        emit ratingChanged(currentFile->second);
    }
}

void RatingSystem::decRating()
{
    if (currentFile->second > Lowest)
    {
        int i = (int)currentFile->second;
        currentFile->second = (Rating)(--i);
        emit ratingChanged(currentFile->second);
    }
}

void RatingSystem::emitChangeSignals()
{
    emit fileChanged(std::distance(files.begin(), currentFile));
    emit ratingChanged(currentFile->second);
}
