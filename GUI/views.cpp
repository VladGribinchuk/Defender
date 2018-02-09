#include "views.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QPushButton>

#include <algorithm>

namespace defender_gui
{

NodesView::NodesView()
{
    setLayout(new QVBoxLayout);
    layout()->addWidget(new QLabel("List of nodes online:"));
    layout()->addWidget(mListWidget = new QListWidget);
}

void NodesView::addNode(const QString& nodeName)
{
    mListWidget->addItem(nodeName);
}

void NodesView::removeNode(const QString& nodeName)
{
    auto found = mListWidget->findItems(nodeName, Qt::MatchExactly);
    for (const auto& item : found)
        delete mListWidget->takeItem(mListWidget->row(item));
}

void NodesView::clearNodes()
{
    while(mListWidget->count() > 0)
        delete mListWidget->takeItem(0);
}

ProcessesView::ProcessesView()
{
    setLayout(new QVBoxLayout);
    ((QVBoxLayout*)layout())->addWidget(new QLabel("PID:\t\tProcess name:"));
    ((QVBoxLayout*)layout())->addWidget(mProcessView = new QListWidget, 1);
    auto stopButton = new QPushButton("Stop process");
    ((QVBoxLayout*)layout())->addWidget(stopButton, 1);
    connect(stopButton, &QPushButton::clicked, this, [this](){
        if (mProcessView->currentItem())
            emit stopProcess(defender_engine::ProcessInfo::parsePID(mProcessView->currentItem()->text()));
    });
}

void ProcessesView::updateProcessesList(const defender_engine::ProcessesList& processes)
{
    long pid = -1;
    if (mProcessView->currentItem())
        pid = defender_engine::ProcessInfo::parsePID(mProcessView->currentItem()->text());

    mProcessView->clear();
    for (const auto& process : processes)
    {
        mProcessView->addItem(process.toString());
        if (pid == process.pid)
        {
            mProcessView->setCurrentRow(mProcessView->count() - 1);
            mProcessView->scrollToItem(mProcessView->item(mProcessView->count() - 1));
        }
    }
}

SettingsView::SettingsView()
{
    setLayout(new QFormLayout);
    mPort = new QTextEdit();
    mPort->setFixedSize(100, 25);
    ((QFormLayout*)layout())->addRow("Port: ", mPort);

    mTimeout = new QTextEdit();
    mTimeout->setFixedSize(100, 25);
    ((QFormLayout*)layout())->addRow("Timeout: ", mTimeout);

    mStartUp = new QCheckBox();
    ((QFormLayout*)layout())->addRow("StartUp: ", mStartUp);

    ((QFormLayout*)layout())->addRow("System info: ", mSysInfo = new QLabel());
}

void SettingsView::setData(int port, int timeout, bool atStartUp, const QString& sysInfo)
{
    mPort->setText(QString::number(port));
    mTimeout->setText(QString::number(timeout));
    mStartUp->setChecked(atStartUp);
    mSysInfo->setText(sysInfo);

    connect(mPort, &QTextEdit::textChanged, this, [this](){
        emit portChanged(mPort->toPlainText().toInt());
    });

    connect(mTimeout, &QTextEdit::textChanged, this, [this](){
        emit timeoutChanged(mTimeout->toPlainText().toInt());
    });

    connect(mStartUp, &QCheckBox::toggled, this, &SettingsView::startUpChanged);
}


}
