#include "logger.h"

#include <QString>
#include <QDateTime>

namespace defender_engine
{

Logger::Logger() :
    mFile("defender.log")
{
    mFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
}

void Logger::log(LogType type, const QString& msg)
{
    emit logging(msg);

    if (!mFile.isOpen())
        return;

    QString entry;
    entry += toQString(type) + ": ";
    entry += QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") + " ";
    entry += msg + "\n";
    mFile.write(entry.toUtf8());
    mFile.flush();
}

void Logger::message(const QString& msg)
{
    log(LogType::Message, msg);
}

void Logger::warning(const QString& msg)
{
    log(LogType::Warning, msg);
}

void Logger::error(const QString& msg)
{
    log(LogType::Error, msg);
}

}
