TEMPLATE = lib
QT -= gui
TARGET = paintfield-minizip

DEFINES += unix

mac {
    DEFINES += USE_FILE32API
}

include(../../src.pri)

LIBS += -lz

HEADERS += \
    zip.h \
    unzip.h \
    mztools.h \
    ioapi.h \
    crypt.h

SOURCES += \
    zip.c \
    unzip.c \
    mztools.c \
    ioapi.c

