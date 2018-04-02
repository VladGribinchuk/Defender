#include "fileutil.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QThread>

namespace defender_engine
{

FilesInfo FileUtil::checkDirectory(const QString& dir)
{
    FilesInfo result;
    QDirIterator it(dir, QStringList() << "*.*", QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        if (QThread::currentThread()->isInterruptionRequested())
            return result;
        QString path = it.next();
        QFileInfo fi(path);
        if (fi.isFile())
        {
            if (fi.isExecutable())
                result.exeFiles.push_back(FileInfo{path});
            result.totalFilesCount++;
        }
        else if (fi.isDir())
            result.totalDirCount++;
    }
    return result;
}

}
