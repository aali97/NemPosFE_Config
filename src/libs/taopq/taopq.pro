include(../../library.pri)

QT       -= core gui

DEFINES += TAOPQ_LIBRARY

CONFIG += c++17

INCLUDEPATH += \
            $$PWD/include

PQ_PATH = $$3RDPARTYLIBS_DIR/pq
PQ_INCLUDE_PATH = $$PQ_PATH/include

unix|win32: LIBS += -L$$PQ_PATH/lib/ -llibpq

INCLUDEPATH += $$PQ_INCLUDE_PATH
DEPENDPATH += $$PQ_INCLUDE_PATH

SOURCES += \
        src/internal/demangle.cpp \
        src/internal/printf.cpp \
        src/internal/strtox.cpp \
        src/connection.cpp \
        src/connection_pool.cpp \
        src/field.cpp \
        src/result.cpp \
        src/result_traits.cpp \
        src/row.cpp \
        src/table_writer.cpp \
        src/transaction.cpp

HEADERS += \
        include/internal/demangle.h \
        include/internal/pool.h \
        include/internal/printf.h \
        include/internal/strtox.h \
        include/connection.h \
        include/connection_pool.h \
        include/field.h \
        include/null.h \
        include/parameter_traits.h \
        include/pq.h \
        include/result.h \
        include/result_traits.h \
        include/result_traits_optional.h \
        include/result_traits_pair.h \
        include/result_traits_tuple.h \
        include/row.h \
        include/table_writer.h \
        include/transaction.h
