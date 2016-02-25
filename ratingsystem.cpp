#include "ratingsystem.h"

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



void RatingSystem::setDir(QDir dir)
{
    QDirIterator it(dir.absolutePath(),
                    QStringList{"*.jpg","*.jpeg","*.png","*.bmp"},
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    files.clear();

    for (it.next(); it.hasNext(); it.next())
    {
        files.push_back({it.fileInfo(), Rating::Medium});
    }

    std::sort(files.begin(), files.end(), [](const auto &f1, const auto &f2) -> bool {return f1.first.fileName() < f2.first.fileName();});
}

void RatingSystem::setActiveFile(int index)
{
    currentFile = std::next(files.begin(), index);
    emit ratingChanged(currentFile->second);
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
