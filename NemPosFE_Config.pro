QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NemPosFE_Config
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

INCLUDEPATH += \
        $$PWD/include/

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp

HEADERS += \
        include/mainwindow.h

FORMS += \
        ui/mainwindow.ui
