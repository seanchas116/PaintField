#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T21:53:53
#
#-------------------------------------------------

QT       += core gui

TARGET = PaintField
TEMPLATE = app

include(../src.pri)

QMAKE_CXXFLAGS += -fopenmp -std=c++0x -m64 -Wno-cpp
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6

CONFIG(release) {
	#QMAKE_CXXFLAGS += -O3
} else {
	#QMAKE_CXXFLAGS += -O0
}

mac {
	#SHARED_LIBS += "$$PWD/lib/lib/libzip.2.dylib" "$$PWD/lib/lib/libfreeimage-3.15.1.dylib"
	#SHARED_LIBS_DESTDIR += "$$DESTDIR/$$join(TARGET,,,.app)/Contents/Frameworks"
	
	#for (FILE, SHARED_LIBS) {
	#	QMAKE_POST_LINK += $$quote(cp $${FILE} $${SHARED_LIBS_DESTDIR}$$escape_expand(\n\t))
	#}
	
	#sharedlibs.path = "$$DESTDIR/$$join(TARGET,,,.app)/Contents/Frameworks"
	#sharedlibs.files += "$$PWD/lib/lib/libzip.2.dylib"
	#sharedlibs.files += "$$PWD/lib/lib/libfreeimage-3.15.1.dylib"
	#INSTALLS += sharedlibs

	#sharedlibs.path = "Contents/Frameworks"
	#sharedlibs.files += "$$PWD/lib/lib/libzip.2.dylib"
	#sharedlibs.files += "$$PWD/lib/lib/libfreeimage-3.15.1.dylib"
	#QMAKE_BUNDLE_DATA += sharedlibs
}

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../libs
LIBS += -lfreeimage -lz
LIBS += $$OUT_PWD/../libs/Malachite/libmalachite.a
LIBS += $$OUT_PWD/../libs/Minizip/libminizip.a
LIBS += $$OUT_PWD/../libs/QJson/libqjson.a

CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
}

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    readme

HEADERS += \
    core/toolmanager.h \
    core/tool.h \
    core/thumbnail.h \
    core/tabletinputdata.h \
    core/tabletevent.h \
    core/tabletapplication.h \
    core/scopedtimer.h \
    core/randomstring.h \
    core/palettemanager.h \
    core/layerrenderer.h \
    core/layermodel.h \
    core/layeredit.h \
    core/layer.h \
    core/global.h \
    core/drawutil.h \
    core/document.h \
    core/application.h \
    core/dialogs/newdocumentdialog.h \
    core/dialogs/exportdialog.h \
    core/internal/tableteventfilter.h \
    core/internal/layermodelcommand.h \
    core/widgets/widgetgroup.h \
    core/widgets/simplebutton.h \
    core/widgets/modulardoublespinbox.h \
    core/widgets/loosespinbox.h \
    core/widgets/doubleslider.h \
    core/widgets/colorwheel.h \
    core/widgets/colorslider.h \
    core/widgets/colorbutton.h \
    core/documentio.h \
    core/canvascontroller.h \
    core/workspacecontroller.h \
    core/workspaceview.h \
    core/workspacemanager.h \
    core/canvasview.h \
    core/actionmanager.h \
    core/util.h \
    extension/layermodelview.h \
    extension/layermodelviewdelegate.h \
    core/workspacemdiareacontroller.h \
    extension/layeractioncontroller.h \
    core/sidebarfactory.h \
    extension/colorsidebar.h \
    extension/colorsidebarfactory.h \
    extension/layertreesidebarfactory.h \
    extension/layertreesidebar.h \
    extension/toolsidebar.h \
    extension/toolsidebarfactory.h \
    extension/toolsettingsidebarfactory.h \
    extension/toolsettingsidebar.h \
    core/module.h \
    extension/extensionmodule.h

SOURCES += \
    core/toolmanager.cpp \
    core/thumbnail.cpp \
    core/tabletapplication.cpp \
    core/randomstring.cpp \
    core/palettemanager.cpp \
    core/layerrenderer.cpp \
    core/layermodel.cpp \
    core/layeredit.cpp \
    core/layer.cpp \
    core/drawutil.cpp \
    core/document.cpp \
    core/application.cpp \
    core/dialogs/newdocumentdialog.cpp \
    core/dialogs/exportdialog.cpp \
    core/internal/tableteventfilter.cpp \
    core/internal/layermodelcommand.cpp \
    core/widgets/widgetgroup.cpp \
    core/widgets/simplebutton.cpp \
    core/widgets/modulardoublespinbox.cpp \
    core/widgets/loosespinbox.cpp \
    core/widgets/doubleslider.cpp \
    core/widgets/colorwheel.cpp \
    core/widgets/colorslider.cpp \
    core/widgets/colorbutton.cpp \
    main.cpp \
    core/documentio.cpp \
    core/canvascontroller.cpp \
    core/workspacecontroller.cpp \
    core/workspaceview.cpp \
    core/workspacemanager.cpp \
    core/canvasview.cpp \
    core/actionmanager.cpp \
    core/util.cpp \
    extension/layermodelview.cpp \
    extension/layermodelviewdelegate.cpp \
    core/workspacemdiareacontroller.cpp \
    extension/layeractioncontroller.cpp \
    extension/colorsidebar.cpp \
    extension/colorsidebarfactory.cpp \
    extension/layertreesidebarfactory.cpp \
    extension/layertreesidebar.cpp \
    extension/toolsidebar.cpp \
    extension/toolsidebarfactory.cpp \
    extension/toolsettingsidebarfactory.cpp \
    extension/toolsettingsidebar.cpp \
    extension/extensionmodule.cpp

FORMS += \
    core/dialogs/newdocumentdialog.ui \
    core/dialogs/exportdialog.ui




