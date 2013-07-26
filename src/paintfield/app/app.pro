TEMPLATE = app
TARGET = PaintField

include(../paintfield-exec.pri)
include(../postlink.pri)

DEFINES += "PF_VERSION=$$VERSION"

SOURCES += main.cpp

OTHER_FILES += \
    Info.plist
