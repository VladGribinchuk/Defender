#ifndef NODESVIEW_H
#define NODESVIEW_H

#include <QWidget>

#include "processutil.h"

class QListWidget;
class QTextEdit;
class QCheckBox;
class QLabel;
class QPushButton;
class QSlider;

namespace defender_gui
{

class NodesView : public QWidget
{
    Q_OBJECT

public:
    NodesView();

    void setCurrentIP(const QString&);
    void setCurrentStatus(const QString&);
    void updateCurrentStatusInfo();
    void addNode(const QString&);
    void removeNode(const QString&);
    void modifyNode(const QString&, const QString&);
    void clearNodes();

signals:
    void stopCurrentTask();

private:
    QLabel* mIPLabel;
    QLabel* mStatusLabel;
    QString mStatusText;
    QListWidget* mListWidget;
    QPushButton* mStopButton;
    long long counter = 0;
};

class ProcessesView : public QWidget
{
    Q_OBJECT

public:
    ProcessesView();

    void updateProcessesList(const defender_engine::ProcessesList& processes);

signals:
    void stopProcess(long);

private:
    QListWidget* mProcessView;
};

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    SettingsView();

    void setData(int port, int timeout, bool atStartUp, const QString&, int workPeriod);
    void setScanningDir(const QString& dir);

signals:
    void portChanged(int);
    void timeoutChanged(int);
    void startUpChanged(bool);
    void scanningDirChanged(const QString&);
    void periodChanged(int);

private:
    QTextEdit* mPort;
    QTextEdit* mTimeout;
    QCheckBox* mStartUp;
    QLabel* mSysInfo;
    QLabel* mScanningDir;
    QPushButton* mScanningButton;
    QSlider* mSlider;
    QLabel* mWorkPeriodLabel;
};

}

#endif // NODESVIEW_H
