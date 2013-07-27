#-------------------------------------------------
#
# Project created by QtCreator 2013-02-05T21:17:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

include(../malachite-exec.pri)

SOURCES += main.cpp \
    viewport.cpp \
    form.cpp

HEADERS  += \
    viewport.h \
    form.h

FORMS += \
    form.ui

RESOURCES += \
    demo.qrc
