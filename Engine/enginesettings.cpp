#include "enginesettings.h"

namespace defender_engine
{

EngineSettings::EngineSettings() :
    mSettings("defender.ini", QSettings::IniFormat)
{
}

QVariant EngineSettings::value(const QString& key, const QVariant& defaultValue) const
{
    return mSettings.value(key, defaultValue);
}

void EngineSettings::setValue(const QString& key, const QVariant& val)
{
    return mSettings.setValue(key, val);
}

}
