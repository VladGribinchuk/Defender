#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QString>

namespace defender_engine
{

struct LogInfo
{
    long deletedEntries = 0;
    long sizeBefore = 0;
    long sizeAfter = 0;
};

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
    void critical(const QString&);
    void state(const QString&);

    LogInfo optimize();

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
        Error,
        CriticalIssue,
        WorkingState
    };

    static QString toQString(LogType type)
    {
        switch (type)
        {
        case LogType::Message: return QString("Message");
        case LogType::Warning: return QString("Warning");
        case LogType::Error: return QString("Error");
        case LogType::CriticalIssue: return QString("CriticalIssue");
        case LogType::WorkingState: return QString("WorkingState");
        default: return QString("");
        }
    }

    void log(LogType, const QString&);

private:
    QFile mFile;
};

}

#endif // LOGGER_H
