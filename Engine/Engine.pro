QT       += core network

QMAKE_CXXFLAGS += -std=c++11
TARGET = Engine
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    logger.cpp \
    processutil.cpp \
    startuputil.cpp \
    systeminfoutil.cpp \
    defenderengine.cpp \
    defenderenginesettings.cpp
HEADERS += \
    logger.h \
    processutil.h \
    startuputil.h \
    systeminfoutil.h \
    defenderengine.h \
    defenderenginesettings.h

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
