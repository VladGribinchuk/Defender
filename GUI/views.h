#ifndef NODESVIEW_H
#define NODESVIEW_H

#include <QWidget>

#include "processutil.h"

class QListWidget;
class QTextEdit;
class QCheckBox;
class QLabel;

namespace defender_gui
{

class NodesView : public QWidget
{
    Q_OBJECT

public:
    NodesView();

    void addNode(const QString&);
    void removeNode(const QString&);
    void clearNodes();

private:
    QListWidget* mListWidget;
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

    void setData(int port, int timeout, bool atStartUp, const QString&);

signals:
    void portChanged(int);
    void timeoutChanged(int);
    void startUpChanged(bool);

private:
    QTextEdit* mPort;
    QTextEdit* mTimeout;
    QCheckBox* mStartUp;
    QLabel* mSysInfo;
};

}

#endif // NODESVIEW_H
