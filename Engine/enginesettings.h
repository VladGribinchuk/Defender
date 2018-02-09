#ifndef ENGINESETTINGS_H
#define ENGINESETTINGS_H

#include <QVariant>
#include <QString>
#include <QSettings>

namespace defender_engine
{

class EngineSettings
{
public:
    static EngineSettings& instance()
    {
        static EngineSettings inst;
        return inst;
    }

    EngineSettings(const EngineSettings&) = delete;
    EngineSettings& operator=(const EngineSettings&) = delete;

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString& key, const QVariant& val);

private:
    EngineSettings();

private:
    QSettings mSettings;
};

}

#endif // ENGINESETTINGS_H
