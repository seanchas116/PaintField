TEMPLATE = app
TARGET = PaintField

include(../paintfield.pri)
include(../postlink.pri)

DEFINES += "PAINTFIELD_VERSION=$$PAINTFIELD_VERSION"

SOURCES += main.cpp

RESOURCES +=

OTHER_FILES += \
    Info.plist
