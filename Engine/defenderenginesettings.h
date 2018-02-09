#ifndef ENGINESETTINGS_H
#define ENGINESETTINGS_H

#include <QVariant>
#include <QString>
#include <QSettings>

namespace defender_engine
{

class DefenderEngineSettings
{
public:
    static DefenderEngineSettings& instance()
    {
        static DefenderEngineSettings inst;
        return inst;
    }

    DefenderEngineSettings(const DefenderEngineSettings&) = delete;
    DefenderEngineSettings& operator=(const DefenderEngineSettings&) = delete;

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString& key, const QVariant& val);

private:
    DefenderEngineSettings();

private:
    QSettings mSettings;
};

}

#endif // ENGINESETTINGS_H
