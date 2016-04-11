#-------------------------------------------------
#
# Project created by QtCreator 2016-02-01T12:30:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = radarStationEmulator
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    ../common/UdpSocketThread.cpp \
    radarstationemul.cpp \
    ../radarStationPrototype/rscontroller/rsprotocols/radarstationprotocols.cpp \
    ../radarStationPrototype/rscontroller/rssockets/udpsocket.cpp

HEADERS  += dialog.h \
    ../common/UdpSocketThread.h \
    radarstationemul.h \
    ../radarStationPrototype/rscontroller/rsprotocols/radarstationprotocols.h \
    ../radarStationPrototype/rscontroller/rssockets/udpsocket.h

FORMS    += dialog.ui
