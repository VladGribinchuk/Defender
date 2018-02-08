QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
TARGET = DefenderApp
TEMPLATE = app
INCLUDEPATH += ../
debug:LIBS += -L$$OUT_PWD/../GUI/debug -lGUI
release:LIBS += -L$$OUT_PWD/../GUI/release -lGUI

SOURCES += main.cpp
