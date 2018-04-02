#include "logger.h"

#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>

#include <vector>

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

void Logger::critical(const QString& msg)
{
    log(LogType::CriticalIssue, msg);
}

void Logger::state(const QString& msg)
{
    log(LogType::WorkingState, msg);
}

LogInfo Logger::optimize()
{
    LogInfo res;

    mFile.close();

    {
        QFileInfo fi("defender.log");
        res.sizeBefore = fi.size();
    }

    QStringList itemsToRemove;
    itemsToRemove << toQString(Logger::LogType::Message);

    QString pattern;
    for (int i = 0; i < itemsToRemove.size(); ++i)
    {
        if (i == itemsToRemove.size() - 1)
            pattern += itemsToRemove[i];
        else
            pattern += itemsToRemove[i] + "|";
    }
    QRegExp regexp(pattern);

    std::vector<QString> entries;
    if (mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&mFile);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains(regexp))
                res.deletedEntries++;
            else
                entries.push_back(line);
        }
        mFile.close();
    }

    if (mFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream stream(&mFile);
        for (const auto entry : entries)
        {
            stream << entry << "\n";
        }
        mFile.close();
    }

    {
        QFileInfo fi("defender.log");
        res.sizeAfter = fi.size();
    }

    mFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);

    return res;
}

}
