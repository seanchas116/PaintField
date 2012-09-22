TEMPLATE = lib
CONFIG += staticlib
QT -= gui
TARGET = QJson

include(../../src.pri)

HEADERS += \
    stack.hh \
    serializerrunnable.h \
    serializer.h \
    Serializer \
    qobjecthelper.h \
    QObjectHelper \
    qjson_export.h \
    qjson_debug.h \
    position.hh \
    parserrunnable.h \
    parser.h \
    parser_p.h \
    Parser \
    location.hh \
    json_scanner.h \
    json_parser.hh

SOURCES += \
    serializerrunnable.cpp \
    serializer.cpp \
    qobjecthelper.cpp \
    parserrunnable.cpp \
    parser.cpp \
    json_scanner.cpp \
    json_parser.cc


