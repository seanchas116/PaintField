TEMPLATE = app
TARGET = PaintFieldTest

include(../paintfield-exec.pri)

QT += testlib

SOURCES += main.cpp \
    testutil.cpp \
    test_shapelayer.cpp \
    test_rectlayer.cpp \
    test_librarymodel.cpp \
    test_layerscene.cpp \
    test_json.cpp \
    test_documentio.cpp \
    test_document.cpp \
    autotest.cpp \
    test_zipunzip.cpp

HEADERS += \
    testutil.h \
    test_shapelayer.h \
    test_rectlayer.h \
    test_librarymodel.h \
    test_layerscene.h \
    test_json.h \
    test_documentio.h \
    test_document.h \
    autotest.h \
    test_zipunzip.h
