#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QString>

namespace defender_engine
{

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance()
    {
        static Logger inst;
        return inst;
    }

    void message(const QString&);
    void warning(const QString&);
    void error(const QString&);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

signals:
    void logging(const QString&);

private:
    Logger();

    enum class LogType
    {
        Message,
        Warning,
        Error
    };

    static QString toQString(LogType type)
    {
        switch (type)
        {
        case LogType::Message: return QString("Message");
        case LogType::Warning: return QString("Warning");
        case LogType::Error: return QString("Error");
        default: return QString("");
        }
    }

    void log(LogType, const QString&);

private:
    QFile mFile;
};

}

#endif // LOGGER_H
