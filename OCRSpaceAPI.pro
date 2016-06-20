#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T14:11:37
#
#-------------------------------------------------
QT       += qml quick widgets
QT       += core gui
QT       += core network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = OCRSpaceAPI
TEMPLATE = app

CONFIG  += c++11
SOURCES += main.cpp\
        mainwindow.cpp \
    flowlayout.cpp

HEADERS  += mainwindow.h \
    flowlayout.h

FORMS    += mainwindow.ui


