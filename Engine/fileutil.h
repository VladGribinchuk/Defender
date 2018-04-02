#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QString>
#include <vector>

namespace defender_engine
{


struct FileInfo
{
    QString path;
};

struct FilesInfo
{
    long totalFilesCount = 0;
    long totalDirCount = 0;
    std::vector<FileInfo> exeFiles;
};

class FileUtil
{
public:
    FileUtil() = default;

    static FilesInfo checkDirectory(const QString&);

};

}

#endif // FILEUTIL_H
