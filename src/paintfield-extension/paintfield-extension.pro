######################################################################
# Automatically generated by qmake (2.01a) ? 10? 28 13:02:54 2012
######################################################################

TEMPLATE = lib
TARGET = paintfield-extension

include(../src.pri)

QMAKE_CXXFLAGS += -std=c++11

mac {
  #QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/
}

INCLUDEPATH += $$PWD $$PWD/../ $$PWD/../libs $$PWD/../libs/Malachite/include

LIBS += -L$$OUT_PWD/../libs/Malachite/src -L$$OUT_PWD/../libs/Minizip
LIBS += -lfreeimage -lz -lmalachite -lminizip -lqjson

LIBS += -L$$OUT_PWD/../paintfield-core
LIBS += -lpaintfield-core

# Input
HEADERS += \
           colorui/colorsidebar.h \
           colorui/coloruimodule.h \
           layerui/layermodelview.h \
           layerui/layermodelviewdelegate.h \
           layerui/layertreesidebar.h \
           layerui/layeruimodule.h \
           toolui/tooluimodule.h \
           basictool/brush/brushtool.h \
           basictool/brush/brushtoolmodule.h \
           basictool/move/layermovetool.h \
    basictool/move/layermovetoolmodule.h \
    extensionmodulefactory.h \
    navigator/navigatorview.h \
    navigator/navigatorcontroller.h \
    navigator/navigatormodule.h \
    layerui/layeruicontroller.h \
    basictool/brush/brushlibrarymodel.h \
    basictool/brush/brushlibrarycontroller.h \
    basictool/brush/brushlibraryview.h \
    basictool/brush/brushpresetmanager.h \
    basictool/brush/brushstroker.h \
    basictool/brush/brushstrokerfactorymanager.h \
    basictool/brush/brushstrokerpen.h \
    basictool/brush/brushstrokercustombrush.h \
    basictool/brush/brushpreferencesmanager.h \
    basictool/brush/brushsidebar.h \
    basictool/brush/brushstrokingthread.h
SOURCES += \
           colorui/colorsidebar.cpp \
           colorui/coloruimodule.cpp \
           layerui/layermodelview.cpp \
           layerui/layermodelviewdelegate.cpp \
           layerui/layertreesidebar.cpp \
           layerui/layeruimodule.cpp \
           toolui/tooluimodule.cpp \
           basictool/brush/brushtool.cpp \
           basictool/brush/brushtoolmodule.cpp \
           basictool/move/layermovetool.cpp \
    basictool/move/layermovetoolmodule.cpp \
    extensionmodulefactory.cpp \
    navigator/navigatorview.cpp \
    navigator/navigatorcontroller.cpp \
    navigator/navigatormodule.cpp \
    layerui/layeruicontroller.cpp \
    basictool/brush/brushlibrarymodel.cpp \
    basictool/brush/brushlibrarycontroller.cpp \
    basictool/brush/brushlibraryview.cpp \
    basictool/brush/brushpresetmanager.cpp \
    basictool/brush/brushstroker.cpp \
    basictool/brush/brushstrokerfactorymanager.cpp \
    basictool/brush/brushstrokerpen.cpp \
    basictool/brush/brushstrokercustombrush.cpp \
    basictool/brush/brushpreferencesmanager.cpp \
    basictool/brush/brushsidebar.cpp \
    basictool/brush/brushstrokingthread.cpp

RESOURCES += \
    resources/resource-paintfield-extension.qrc
