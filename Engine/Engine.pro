QT       += core network

QMAKE_CXXFLAGS += -std=c++11
TARGET = Engine
TEMPLATE = lib
CONFIG += staticlib

debug:LIBS += -L$$OUT_PWD/../DataTransmission/debug -lDataTransmission
release:LIBS += -L$$OUT_PWD/../DataTransmission/release -lDataTransmission
INCLUDEPATH += ../

SOURCES += engine.cpp
HEADERS += engine.h
