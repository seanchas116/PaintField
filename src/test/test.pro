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

INCLUDEPATH += $$PWD $$PWD/../ $$PWD/../libs $$PWD/../libs/Malachite/include

LIBS += -L$$OUT_PWD/../libs/Malachite/src -L$$OUT_PWD/../libs/Minizip
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
	QMAKE_BUNDLE_DATA += sharedlibs
}

SOURCES += main.cpp \
    autotest.cpp \
    test_documentio.cpp \
    test_document.cpp \
    testutil.cpp \
    testobject.cpp \
    splittabareatestwidget.cpp \
    test_librarymodel.cpp

HEADERS += \
    autotest.h \
    test_documentio.h \
    test_document.h \
    testutil.h \
    testobject.h \
    splittabareatestwidget.h \
    test_librarymodel.h
