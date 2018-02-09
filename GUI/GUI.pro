QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
TARGET = GUI
TEMPLATE = lib
CONFIG += shared

SOURCES += mainwindow.cpp \
    views.cpp
HEADERS += mainwindow.h \
    defender_gui_global.h \
    views.h

RESOURCES += resources.qrc

DEFINES += DEFENDER_GUI_LIBRARY

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Engine/release/ -lEngine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Engine/debug/ -lEngine
else:unix: LIBS += -L$$OUT_PWD/../Engine/ -lEngine

INCLUDEPATH += $$PWD/../Engine
DEPENDPATH += $$PWD/../Engine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Engine/release/libEngine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Engine/debug/libEngine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Engine/release/Engine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Engine/debug/Engine.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Engine/libEngine.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DataTransmission/release/ -lDataTransmission
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DataTransmission/debug/ -lDataTransmission
else:unix: LIBS += -L$$OUT_PWD/../DataTransmission/ -lDataTransmission

INCLUDEPATH += $$PWD/../DataTransmission
DEPENDPATH += $$PWD/../DataTransmission

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataTransmission/release/libDataTransmission.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataTransmission/debug/libDataTransmission.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataTransmission/release/DataTransmission.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataTransmission/debug/DataTransmission.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../DataTransmission/libDataTransmission.a

win32:LIBS += -lpsapi

