QT       += core network

QMAKE_CXXFLAGS += -std=c++11
TARGET = DataTransmission
TEMPLATE = lib
CONFIG += staticlib

SOURCES += datatransmission.cpp \
    message.cpp \
    messagereceiver.cpp \
    messagesender.cpp
HEADERS += datatransmission.h \
    message.h \
    messagereceiver.h \
    messagesender.h
