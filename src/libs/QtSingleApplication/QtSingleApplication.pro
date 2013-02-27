TEMPLATE = lib
TARGET = paintfield-qtsingleapplication

QT += network

INCLUDEPATH += .

include(../../src.pri)

# Input
HEADERS += qtlocalpeer.h \
           qtlockedfile.h \
           qtsingleapplication.h \
           qtlockedfile.cpp \
           qtlockedfile_win.cpp \
           qtlockedfile_unix.cpp
SOURCES += qtlocalpeer.cpp \
           qtlockedfile.cpp \
           qtlockedfile_unix.cpp \
           qtlockedfile_win.cpp \
           qtsingleapplication.cpp
