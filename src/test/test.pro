#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T20:14:05
#
#-------------------------------------------------

TEMPLATE = app
QT += core gui testlib
TARGET = test

include(../src.pri)

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD $$PWD/../ $$PWD/../libs $$PWD/../libs/Malachite/include $$PWD/../libs/qjson/include

LIBS += -L$$OUT_PWD/../libs/Malachite/src -L$$OUT_PWD/../libs/Minizip -L$$OUT_PWD/../libs/qjson/lib
LIBS += -lfreeimage -lz -lmalachite -lminizip -lqjson

LIBS += -L$$OUT_PWD/../paintfield-core
LIBS += -lpaintfield-core

LIBS += -L$$OUT_PWD/../paintfield-extension
LIBS += -lpaintfield-extension

mac {
	sharedlibs.path = "Contents/MacOS"
	sharedlibs.files += $$OUT_PWD/../paintfield-core/libpaintfield-core.1.dylib
	sharedlibs.files += $$OUT_PWD/../paintfield-extension/libpaintfield-extension.1.dylib
	sharedlibs.files += $$OUT_PWD/../libs/Malachite/src/libmalachite.1.dylib
	sharedlibs.files += $$OUT_PWD/../libs/Minizip/libminizip.1.dylib
	sharedlibs.files += $$OUT_PWD/../libs/qjson/lib/libqjson.0.dylib
	QMAKE_BUNDLE_DATA += sharedlibs
}

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
