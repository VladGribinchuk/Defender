QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
TARGET = DefenderApp
TEMPLATE = app
INCLUDEPATH += ../

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GUI/release/ -lGUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GUI/debug/ -lGUI
else:unix: LIBS += -L$$OUT_PWD/../GUI/ -lGUI

INCLUDEPATH += $$PWD/../GUI
DEPENDPATH += $$PWD/../GUI
