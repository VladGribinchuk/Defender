#include "views.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSlider>

#include <algorithm>

namespace defender_gui
{

NodesView::NodesView()
{
    setLayout(new QVBoxLayout);

    QHBoxLayout* hbl = new QHBoxLayout(this);
    hbl->addWidget(new QLabel("Local IP:"));
    hbl->addWidget(mIPLabel = new QLabel);
    ((QVBoxLayout*)layout())->addLayout(hbl);
    QHBoxLayout* hbl2 = new QHBoxLayout(this);
    hbl2->addWidget(new QLabel("Current status:"));
    hbl2->addWidget(mStatusLabel = new QLabel(""));
    ((QVBoxLayout*)layout())->addLayout(hbl2);

    layout()->addWidget(mStopButton = new QPushButton("Stop current task"));
    layout()->addWidget(new QLabel("Current status:"));
    layout()->addWidget(new QLabel("List of nodes online:"));
    layout()->addWidget(mListWidget = new QListWidget);

    connect(mStopButton, &QPushButton::clicked, [this](){
        emit stopCurrentTask();
    });
}

void NodesView::setCurrentIP(const QString& ip)
{
    mIPLabel->setText(ip);
}

void NodesView::setCurrentStatus(const QString& status)
{
    mStatusLabel->setText(status);
    mStatusText = status;
    counter = 0;
}

void NodesView::updateCurrentStatusInfo()
{
    ++counter;
    long long mins = counter / 60;
    long long secs = counter % 60;
    mStatusLabel->setText(mStatusText + " " + QString::number(mins) + ":" + QString::number(secs));
}

void NodesView::addNode(const QString& nodeName)
{
    mListWidget->addItem("[" + nodeName + "]: getting info about current status...");
}

void NodesView::removeNode(const QString& nodeName)
{
    auto found = mListWidget->findItems(nodeName, Qt::MatchContains);
    for (const auto& item : found)
        delete mListWidget->takeItem(mListWidget->row(item));
}

void NodesView::modifyNode(const QString& nodeName, const QString& text)
{
     auto found = mListWidget->findItems(nodeName, Qt::MatchContains);
     for (const auto& item : found)
     {
         QString textBase = item->text();
         textBase = textBase.mid(0, textBase.indexOf("]:") + 3);
         item->setText(textBase + text);
     }
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
    ((QFormLayout*)layout())->addRow(mScanningButton = new QPushButton("Scanning dir"), mScanningDir = new QLabel());
    mScanningDir->setFixedSize(150, 25);
    connect(mScanningButton, &QPushButton::clicked, [this](){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        mScanningDir->setText(dir);
        emit scanningDirChanged(dir);
    });

    QStringList list = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::HomeLocation);
    if (!list.empty())
    {
        mScanningDir->setText(list.front());
        emit scanningDirChanged(list.front());
    }

    ((QFormLayout*)layout())->addRow(mWorkPeriodLabel = new QLabel("Working period: "), mSlider = new QSlider(Qt::Horizontal));
    mSlider->setRange(1, 200);
    connect(mSlider, &QSlider::valueChanged, [this](int val){
        mWorkPeriodLabel->setText("Working period: " + QString::number(val) + " mins");
        emit periodChanged(val);
    });
}

void SettingsView::setData(int port, int timeout, bool atStartUp, const QString& sysInfo, int workPeriod)
{
    mPort->setText(QString::number(port));
    mTimeout->setText(QString::number(timeout));
    mStartUp->setChecked(atStartUp);
    mSysInfo->setText(sysInfo);
    mSlider->setValue(workPeriod);
    mWorkPeriodLabel->setText("Working period: " + QString::number(mSlider->value()) + " mins");

    connect(mPort, &QTextEdit::textChanged, this, [this](){
        emit portChanged(mPort->toPlainText().toInt());
    });

    connect(mTimeout, &QTextEdit::textChanged, this, [this](){
        emit timeoutChanged(mTimeout->toPlainText().toInt());
    });

    connect(mStartUp, &QCheckBox::toggled, this, &SettingsView::startUpChanged);
}

void SettingsView::setScanningDir(const QString& dir)
{
    mScanningDir->setText(dir);
}

}
