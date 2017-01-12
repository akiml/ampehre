#-------------------------------------------------
#
# Project created by QtCreator 2017-01-11T13:54:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = msmonitor
TEMPLATE = app

INCLUDEPATH += $$PWD/include/

SOURCES += src/main.cpp \
        src/gui/mainwindow.cpp \
    	src/gui/QMSMpowerplot.cpp \
    	src/CClient.cpp \
    	src/CComC.cpp \
    	src/CProtocolC.cpp \
    	src/CProtocol.cpp \
    	src/utils.cpp

HEADERS  += include/gui/mainwindow.h \
    	include/gui/QMSMpowerplot.h \
    	include/CClient.hpp \
    	include/CComC.hpp \
    	include/CProtocolC.hpp \
    	include/CProtocol.hpp \
    	include/utils.h

LIBS += -L/usr/local/qwt-6.1.3/lib -lqwt

FORMS    += mainwindow.ui
