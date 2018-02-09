#include "defenderenginesettings.h"

namespace defender_engine
{

DefenderEngineSettings::DefenderEngineSettings() :
    mSettings("defender.ini", QSettings::IniFormat)
{
}

QVariant DefenderEngineSettings::value(const QString& key, const QVariant& defaultValue) const
{
    return mSettings.value(key, defaultValue);
}

void DefenderEngineSettings::setValue(const QString& key, const QVariant& val)
{
    return mSettings.setValue(key, val);
}

}
