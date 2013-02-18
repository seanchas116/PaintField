TEMPLATE = app
TARGET = PaintField

include(../paintfield.pri)
include(../postlink.pri)

VERSION = $$PF_VERSION
DEFINES += "PF_VERSION=$$PF_VERSION"

SOURCES += main.cpp

RESOURCES +=

OTHER_FILES += \
    Info.plist
