#-------------------------------------------------
#
# Project created by QtCreator 2014-04-26T19:54:09
#
#-------------------------------------------------

QT       += core gui serialport

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
    utils/RingBuffer.cpp \
    plugins/WidgetPluginInstance.cpp \
    plugins/graph/GraphWidgetPluginInstance.cpp \
    utils/UnitsUtil.cpp \
    plugins/serialPort/SerialPortPlugin.cpp \
    plugins/serialPort/SerialPortConnectDialog.cpp \
    plugins/label/LabelPlugin.cpp \
    plugins/label/LabelWidgetPluginInstance.cpp \
    plugins/label/LabelWidget.cpp

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
    utils/RingBuffer.h \
    plugins/WidgetPluginInstance.h \
    plugins/graph/GraphWidgetPluginInstance.h \
    utils/UnitsUtil.h \
    plugins/serialPort/SerialPortPlugin.h \
    plugins/serialPort/SerialPortConnectDialog.h \
    plugins/label/LabelPlugin.h \
    plugins/label/LabelWidgetPluginInstance.h \
    plugins/label/LabelWidget.h

FORMS    += MainWindow.ui \
    plugins/fileInput/FileInputConnectDialog.ui \
    plugins/serialPort/SerialPortConnectDialog.ui
