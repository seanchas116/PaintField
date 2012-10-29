#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T20:14:05
#
#-------------------------------------------------

QT += core gui testlib
TARGET = test
TEMPLATE = app

include(../src.pri)

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD $$PWD/../ $$PWD/../libs $$PWD/../libs/Malachite/include $$PWD/../libs/qjson/include

LIBS += -L$$OUT_PWD/../libs/Malachite/src -L$$OUT_PWD/../libs/Minizip -L$$OUT_PWD/../libs/qjson/lib
LIBS += -lfreeimage -lz -lmalachite -lminizip -lqjson

LIBS += -L$$OUT_PWD/../paintfield-core
LIBS += -lpaintfield-core

LIBS += -L$$OUT_PWD/../paintfield-extension
LIBS += -lpaintfield-extension

SOURCES += main.cpp \
    autotest.cpp \
    test_documentio.cpp \
    test_document.cpp \
    testutil.cpp

HEADERS += \
    autotest.h \
    test_documentio.h \
    test_document.h \
    testutil.h
