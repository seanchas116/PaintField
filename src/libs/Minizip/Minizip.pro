TEMPLATE = lib
CONFIG += staticlib
QT -= gui
TARGET = minizip

DEFINES += unix

mac {
    DEFINES += USE_FILE32API
}

include(../../src.pri)

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
    minizip.c \
    miniunz.c \
    ioapi.c
