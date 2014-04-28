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
    commands/CommandBase.cpp \
    commands/AddCommand.cpp \
    commands/SetCommand.cpp \
    plugins/Plugin.cpp \
    plugins/InputPlugin.cpp \
    plugins/PluginManager.cpp \
    plugins/WidgetPlugin.cpp \
    plugins/fileInput/FileInputPlugin.cpp \
    plugins/graph/GraphPlugin.cpp \
    plugins/graph/GraphWidget.cpp \
    plugins/fileInput/FileInputConnectDialog.cpp \
    InputReaderThread.cpp \
    utils/RingBuffer.cpp

HEADERS  += MainWindow.h \
    commands/CommandBase.h \
    commands/AddCommand.h \
    commands/SetCommand.h \
    plugins/Plugin.h \
    plugins/InputPlugin.h \
    plugins/PluginManager.h \
    plugins/WidgetPlugin.h \
    plugins/fileInput/FileInputPlugin.h \
    plugins/graph/GraphPlugin.h \
    plugins/graph/GraphWidget.h \
    plugins/fileInput/FileInputConnectDialog.h \
    InputReaderThread.h \
    utils/RingBuffer.h

FORMS    += MainWindow.ui \
    plugins/fileInput/FileInputConnectDialog.ui
