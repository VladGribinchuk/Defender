#include "startuputil.h"

#include <QString>

#ifdef _WIN32
#include <QSettings>
#include <QCoreApplication>
#endif

namespace defender_engine
{

namespace StartUpUtil
{

void setStartUp(const QString &appName, bool on)
{
#ifdef _WIN32
    if (on)
    {
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.setValue(appName, QCoreApplication::applicationFilePath().replace('/', '\\'));
    }
    else
    {
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.remove(appName);
    }
#endif
}

}

}
