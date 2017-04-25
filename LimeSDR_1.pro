#-------------------------------------------------
#
# Project created by QtCreator 2017-04-15T18:58:41
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -std=c++11 #(or QMAKE_CXXFLAGS += -std=c++0x)
TARGET = LimeSDR_1
TEMPLATE = app

LIBS +=-lSoapySDR -lqwt -lfftw3 -lm
SOURCES += main.cpp\
        mainwindow.cpp \
    limesdr.cpp

HEADERS  += mainwindow.h \
    limesdr.h

FORMS    += mainwindow.ui \
    limesdr.ui
