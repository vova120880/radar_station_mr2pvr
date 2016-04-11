#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T10:31:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = radarStationPrototype
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainwindowcontab.cpp \
    mainwindowrlmtab.cpp \
    mainwindowrptab.cpp \
    mainwindowprlitab.cpp \
    mainwindowvrlitab.cpp \
    rscontroller/radarstationcontroller.cpp \
    rscontroller/rsworkers/rsprotocolworker.cpp \
    rscontroller/rssockets/tcpsocket.cpp \
    rscontroller/rssockets/udpsocket.cpp \
    rscontroller/rsmodel/radarstationmodel.cpp \
    rscontroller/rsprotocols/radarstationprotocols.cpp

HEADERS  += mainwindow.h \
    rscontroller/radarstationcontroller.h \
    rscontroller/rsworkers/rsprotocolworker.h \
    rscontroller/rssockets/tcpsocket.h \
    rscontroller/rssockets/udpsocket.h \
    rscontroller/rsmodel/radarstationmodel.h \
    rscontroller/rsprotocols/radarstationprotocols.h

FORMS    += mainwindow.ui \
    tabui.ui
