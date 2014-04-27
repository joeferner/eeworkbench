#-------------------------------------------------
#
# Project created by QtCreator 2014-04-26T19:54:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eeworkbench
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    plugins/graph/Graph.cpp

HEADERS  += MainWindow.h \
    plugins/graph/Graph.h

FORMS    += MainWindow.ui
