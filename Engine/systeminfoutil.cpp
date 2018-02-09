#include "systeminfoutil.h"
#include <QSysInfo>

#ifdef _WIN32
#include <windows.h>
#endif

QString defender_engine::SystemInfoUtil::getSystemInfo()
{
    QString result;
    result += QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture();
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);
    result += QString("\nTotal RAM: %1 Gb").arg((double)statex.ullTotalPhys / (1024*1024*1024), 0, 'f', 2);
#endif
    return result;
}
