include(app_dependencies.pri)
include(../../NemPosFE_Config.pri)

CONFIG(release, debug|release) {
	DEFINES += NDEBUG
}

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += qtc_runnable c++17

OBJECTS_DIR = $${OUT_PWD}/obj
MOC_DIR = $${OUT_PWD}/obj
RCC_DIR = $${OUT_PWD}/obj
UI_DIR = $${OUT_PWD}/obj

DESTDIR = $$IDE_BIN_PATH
LIBS += -L$$IDE_LIBS_PATH

TARGET = NemPosFE_Config

INCLUDEPATH += \
        $$PWD/include/

PQ_PATH = $$3RDPARTYLIBS_DIR/pq
PQ_INCLUDE_PATH = $$PQ_PATH/include

unix|win32: LIBS += -L$$PQ_PATH/lib/ -llibpq

INCLUDEPATH += $$PQ_INCLUDE_PATH
DEPENDPATH += $$PQ_INCLUDE_PATH

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp

HEADERS += \
        include/mainwindow.h

FORMS += \
        ui/mainwindow.ui
