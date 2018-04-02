#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QString>
#include <memory>

namespace data_transmission
{

enum class EngineStates : char
{
    Waiting = 0,
    CheckingFiles = 1,
    CheckingProcesses = 2,
    ScanningFiles = 3,
    ScanningProcesses = 4,
    OptimizingLogs = 5,
    _NumStates = 6
};

inline QString toString(EngineStates state)
{
    switch (state)
    {
    case EngineStates::Waiting: return "Waiting next task";
    case EngineStates::CheckingFiles: return "Checking files in hard disk";
    case EngineStates::CheckingProcesses: return "Checking running processes";
    case EngineStates::ScanningFiles: return "Scanning files in hard disk";
    case EngineStates::ScanningProcesses: return "Scanning running processes";
    case EngineStates::OptimizingLogs: return "Optimizing stored data";
    default: return "";
    }
}

class StatesMessageData
{
public:
    StatesMessageData() = default;
    virtual ~StatesMessageData() {}

    EngineStates state() const { return mState; }

protected:
    virtual QByteArray toByteArray() const = 0;
    virtual void fromByteArray(const QByteArray& array) = 0;
    friend class Message;

    EngineStates mState;
};

class WaitingMessageData : public StatesMessageData
{
public:
    WaitingMessageData()
    {
        mState = EngineStates::Waiting;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        return QByteArray();
    }

    virtual void fromByteArray(const QByteArray& /*array*/) override
    {
    }
};

class CheckingFilesMessageData : public StatesMessageData
{
public:
    CheckingFilesMessageData()
    {
        mState = EngineStates::CheckingFiles;
    }

    CheckingFilesMessageData(QString dir, long dirs, long files, long exeFiles) :
        mDir(dir),
        mTotalDirsCount(dirs),
        mTotalFilesCount(files),
        mExeFiles(exeFiles)
    {
        mState = EngineStates::CheckingFiles;
    }

    long totalDirsCount() const
    {
        return mTotalDirsCount;
    }

    long totalFilesCount() const
    {
        return mTotalFilesCount;
    }

    long exeFiles() const
    {
        return mExeFiles;
    }

    QString dir() const
    {
        return mDir;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        QString dataStr;
        dataStr += QString::number(static_cast<int>(mState)) + "|";
        dataStr += mDir + "|";
        dataStr += QString::number(mTotalDirsCount) + "|";
        dataStr += QString::number(mTotalFilesCount) + "|";
        dataStr += QString::number(mExeFiles);
        return QByteArray::fromStdString(dataStr.toStdString());
    }

    virtual void fromByteArray(const QByteArray& array) override
    {
        QString result = QString::fromStdString(array.toStdString());
        mState = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        result = result.mid(result.indexOf("|") + 1);
        mDir = result.mid(0, result.indexOf("|"));
        result = result.mid(result.indexOf("|") + 1);
        mTotalDirsCount = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mTotalFilesCount = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mExeFiles = result.toLong();
    }

private:
    long mTotalDirsCount, mTotalFilesCount, mExeFiles;
    QString mDir;
};

class CheckingProcessesMessageData : public StatesMessageData
{
public:
    CheckingProcessesMessageData()
    {
        mState = EngineStates::CheckingProcesses;
    }

    CheckingProcessesMessageData(long runningProcesses) :
        mRunningProcesses(runningProcesses)
    {
        mState = EngineStates::CheckingProcesses;
    }

    long runningProcesses() const
    {
        return mRunningProcesses;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        QString dataStr;
        dataStr += QString::number(static_cast<int>(mState)) + "|";
        dataStr += QString::number(mRunningProcesses);
        return QByteArray::fromStdString(dataStr.toStdString());
    }

    virtual void fromByteArray(const QByteArray& array) override
    {
        QString result = QString::fromStdString(array.toStdString());
        mState = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        result = result.mid(result.indexOf("|") + 1);
        mRunningProcesses = result.toLong();
    }

private:
    long mRunningProcesses;
};

class OptimizingLogsMessageData : public StatesMessageData
{
public:
    OptimizingLogsMessageData()
    {
        mState = EngineStates::OptimizingLogs;
    }

    OptimizingLogsMessageData(long deletedEntries, long sizeBefore, long sizeAfter) :
        mDeletedEntries(deletedEntries),
        mSizeBefore(sizeBefore),
        mSizeAfter(sizeAfter)
    {
        mState = EngineStates::OptimizingLogs;
    }

    long deletedEntries() const
    {
        return mDeletedEntries;
    }

    long sizeBefore() const
    {
        return mSizeBefore;
    }

    long sizeAfter() const
    {
        return mSizeAfter;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        QString dataStr;
        dataStr += QString::number(static_cast<int>(mState)) + "|";
        dataStr += QString::number(mDeletedEntries) + "|";
        dataStr += QString::number(mSizeBefore) + "|";
        dataStr += QString::number(mSizeAfter);
        return QByteArray::fromStdString(dataStr.toStdString());
    }

    virtual void fromByteArray(const QByteArray& array) override
    {
        QString result = QString::fromStdString(array.toStdString());
        mState = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        result = result.mid(result.indexOf("|") + 1);
        mDeletedEntries = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mSizeBefore = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mSizeAfter = result.toLong();
    }

private:
    long mDeletedEntries, mSizeBefore, mSizeAfter;
};

class ScanningFilesMessageData : public StatesMessageData
{
public:
    ScanningFilesMessageData()
    {
        mState = EngineStates::ScanningFiles;
    }

    ScanningFilesMessageData(long scanned, long infected) :
        mScannedFiles(scanned),
        mInfectedFiles(infected)
    {
        mState = EngineStates::ScanningFiles;
    }

    long scannedFiles() const
    {
        return mScannedFiles;
    }

    long infectedFiles() const
    {
        return mInfectedFiles;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        QString dataStr;
        dataStr += QString::number(static_cast<int>(mState)) + "|";
        dataStr += QString::number(mScannedFiles) + "|";
        dataStr += QString::number(mInfectedFiles);
        return QByteArray::fromStdString(dataStr.toStdString());
    }

    virtual void fromByteArray(const QByteArray& array) override
    {
        QString result = QString::fromStdString(array.toStdString());
        mState = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        result = result.mid(result.indexOf("|") + 1);
        mScannedFiles = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mInfectedFiles = result.toLong();
    }

private:
    long mScannedFiles, mInfectedFiles;
};

class ScanningProcessesMessageData : public StatesMessageData
{
public:
    ScanningProcessesMessageData()
    {
        mState = EngineStates::ScanningProcesses;
    }

    ScanningProcessesMessageData(long scanned, long infected) :
        mScannedProcesses(scanned),
        mInfectedProcesses(infected)
    {
        mState = EngineStates::ScanningProcesses;
    }

    long scannedProcesses() const
    {
        return mScannedProcesses;
    }

    long infectedProcesses() const
    {
        return mInfectedProcesses;
    }

protected:
    virtual QByteArray toByteArray() const override
    {
        QString dataStr;
        dataStr += QString::number(static_cast<int>(mState)) + "|";
        dataStr += QString::number(mScannedProcesses) + "|";
        dataStr += QString::number(mInfectedProcesses);
        return QByteArray::fromStdString(dataStr.toStdString());
    }

    virtual void fromByteArray(const QByteArray& array) override
    {
        QString result = QString::fromStdString(array.toStdString());
        mState = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        result = result.mid(result.indexOf("|") + 1);
        mScannedProcesses = result.mid(0, result.indexOf("|")).toLong();
        result = result.mid(result.indexOf("|") + 1);
        mInfectedProcesses = result.toLong();
    }

private:
    long mScannedProcesses, mInfectedProcesses;
};


class Message
{
public:
    enum class MessageType : char
    {
        NonType,
        Activated,
        Deactivated,
        Greetings,
        StatesSwitching,
        StateComplete,
        PollOut,
        CurrentState,
        InfectedFileWasFound,
        InfectedProcessWasFound
    };

    Message() :
        mType(MessageType::NonType)
    {}

    explicit Message(MessageType type) :
        mType(type)
    {}

    void append(const QByteArray& data) { mBody.append(data); }
    void append(const char* data, size_t len) { std::copy(data, data + len, std::back_inserter(mBody));}
    void setFrom(StatesMessageData* msg) { mBody = msg->toByteArray(); }
    std::unique_ptr<StatesMessageData> getStatesMsg() const
    {
        QString result = QString::fromStdString(mBody.toStdString());
        EngineStates state = EngineStates(static_cast<char>(result.mid(0, result.indexOf("|")).toInt()));
        std::unique_ptr<StatesMessageData> msg;
        switch(state)
        {
        case EngineStates::Waiting:
            msg.reset(new WaitingMessageData());
            break;
        case EngineStates::CheckingFiles:
            msg.reset(new CheckingFilesMessageData());
            break;
        case EngineStates::CheckingProcesses:
            msg.reset(new CheckingProcessesMessageData());
            break;
        case EngineStates::OptimizingLogs:
            msg.reset(new OptimizingLogsMessageData());
            break;
        case EngineStates::ScanningFiles:
            msg.reset(new ScanningFilesMessageData());
            break;
        case EngineStates::ScanningProcesses:
            msg.reset(new ScanningProcessesMessageData());
            break;
        }
        msg->fromByteArray(mBody);
        return msg;
    }

    MessageType type() const { return mType; }
    const QByteArray& body() const { return mBody; }
    bool valid() const { return mType != MessageType::NonType; }

    QByteArray& body() { return mBody; }

    QByteArray toQByteArray() const;
    static Message fromQByteArray(const QByteArray& data);

private:
    MessageType mType;
    QByteArray mBody;
};

}

#endif // MESSAGE_H
