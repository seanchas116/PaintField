TEMPLATE = lib
CONFIG += staticlib
QT -= gui
TARGET = Minizip

DEFINES += unix

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

