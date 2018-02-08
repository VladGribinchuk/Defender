#include "mainwindow.h"
#include "DataTransmission/datatransmission.h"
#include "Engine/engine.h"

#include <QMenuBar>

namespace defender_gui
{

using namespace defender_engine;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mEngine(new Engine(data_transmission::GetDefaultPort()))
{
}

MainWindow::~MainWindow()
{

}

}
