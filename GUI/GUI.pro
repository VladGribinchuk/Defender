QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
TARGET = GUI
TEMPLATE = lib

debug:LIBS += -L$$OUT_PWD/../Engine/debug -lEngine
release:LIBS += -L$$OUT_PWD/../Engine/release -lEngine
debug:LIBS += -L$$OUT_PWD/../DataTransmission/debug -lDataTransmission
release:LIBS += -L$$OUT_PWD/../DataTransmission/release -lDataTransmission
INCLUDEPATH += ../

SOURCES += mainwindow.cpp
HEADERS += mainwindow.h
