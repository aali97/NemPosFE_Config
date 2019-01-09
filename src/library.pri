include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$LIBRARY_NAME

CONFIG += shared dll
greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++17
} else {
    QMAKE_CXXFLAGS += -std=c++0x
}

include(../NemPosFE_Config.pri)

CONFIG(static) {
    DESTDIR = $$IDE_LIBS_PATH/static/
}
else {
    DESTDIR = $$IDE_LIBS_PATH/
}

TARGET = $$libraryName($$TARGET)

TEMPLATE = lib

INCLUDEPATH += \
    $$OUT_PWD/ \
    $$OUT_PWD/shared/

OBJECTS_DIR = $${OUT_PWD}/obj
MOC_DIR = $${OUT_PWD}/obj
RCC_DIR = $${OUT_PWD}/obj
UI_DIR = $${OUT_PWD}/obj
