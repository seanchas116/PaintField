TEMPLATE = app
TARGET = test

include(../paintfield.pri)

QT += testlib

SOURCES += main.cpp \
    autotest.cpp \
    test_documentio.cpp \
    test_document.cpp \
    testutil.cpp \
    test_librarymodel.cpp \
    test_rectlayer.cpp \
    test_shapelayer.cpp

HEADERS += \
    autotest.h \
    test_documentio.h \
    test_document.h \
    testutil.h \
    test_librarymodel.h \
    test_rectlayer.h \
    test_shapelayer.h
