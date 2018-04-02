#ifndef PROCESSUTIL_H
#define PROCESSUTIL_H

#include <QString>
#include <vector>
#include <string>
#include <memory>

namespace defender_engine
{

class AccessPrivilege;

struct ProcessInfo
{
    QString name;
    long pid;

    QString toString() const
    {
        return QString("[%1]\t%2").arg(pid).arg(name);
    }

    static long parsePID(const QString& processInfoString)
    {
        if (processInfoString.isEmpty())
            return -1;
        return processInfoString.mid(1, processInfoString.indexOf(']') - 1).toLong();
    }
};

using ProcessesList = std::vector<ProcessInfo>;

class ProcessUtil
{
public:
    static ProcessUtil& instance()
    {
        static ProcessUtil inst;
        return inst;
    }

    ProcessesList getAllRunningProcess() const;
    void killProcess(const QString&) const;
    bool killProcessById(long, QString&) const;

    QString exePath(long pid) const;

    ProcessUtil(const ProcessUtil&) = delete;
    ProcessUtil& operator=(const ProcessUtil&) = delete;

    ~ProcessUtil();

private:
    ProcessUtil();

private:
    std::unique_ptr<AccessPrivilege> mPrivilege;
    mutable bool accessPrivilage = false;
};

}

#endif // PROCESSUTIL_H
