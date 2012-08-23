#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T21:53:53
#
#-------------------------------------------------

QT       += core gui

TARGET = PaintField
TEMPLATE = app

QMAKE_CFLAGS_X86_64 = 
QMAKE_CXXFLAGS_X86_64 = 
QMAKE_OBJECTIVE_CFLAGS_X86_64 = 
QMAKE_LFLAGS_X86_64 = 

QMAKE_CXXFLAGS += -fopenmp -std=c++0x -m64
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

INCLUDEPATH += src/core src/dialog src/graphics src/graphics/include src/gui lib/include
LIBS += -lzip -lfreeimage -lmalachite

CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
}

FORMS    += \
    src/dialog/fsnewdocumentdialog.ui \
    src/dialog/fsexportdialog.ui

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    resources/style.css \
    readme

SOURCES += \
    src/main.cpp \
    src/core/fszip.cpp \
    src/core/fstoolmanager.cpp \
    src/core/fstool.cpp \
    src/core/fsthumbnail.cpp \
    src/core/fstabletinputdata.cpp \
    src/core/fstableteventfilter.cpp \
    src/core/fstabletevent.cpp \
    src/core/fsrasterlayer.cpp \
    src/core/fsrandomstring.cpp \
    src/core/fspalettemanager.cpp \
    src/core/fslayeredit.cpp \
    src/core/fslayer.cpp \
    src/core/fsgrouplayer.cpp \
    src/core/fsdrawutil.cpp \
    src/core/fsdocumentmodel.cpp \
    src/core/fscore.cpp \
    src/core/fscanvas.cpp \
    src/dialog/fsnewdocumentdialog.cpp \
    src/gui/fssimplebutton.cpp \
    src/gui/fspanel.cpp \
    src/gui/fslayertreeviewdelegate.cpp \
    src/gui/fslayertreepanel.cpp \
    src/gui/fsguimain.cpp \
    src/gui/fsdoubleslider.cpp \
    src/gui/fscolorwheel.cpp \
    src/gui/fscolorslider.cpp \
    src/gui/fscolorpanel.cpp \
    src/gui/fscolorbutton.cpp \
    src/gui/fscanvasview.cpp \
    src/gui/fsactionmanager.cpp \
    src/tool/fslayermovetool.cpp \
    src/tool/fsbrushtool.cpp \
    src/tool/fsbrushstroker.cpp \
    src/tool/fsbrushsetting.cpp \
    src/gui/fsaction.cpp \
    src/core/fslayerrenderer.cpp \
    src/gui/fslayertreeview.cpp \
    src/gui/fsundoredoaction.cpp \
    src/core/fsdocumentcommand.cpp \
    src/dialog/fsexportdialog.cpp \
    src/gui/fswidgetgroup.cpp \
    src/gui/fstoolpanel.cpp \
    src/gui/fsborderwidget.cpp \
    src/tool/fsbrushsettingwidget.cpp \
    src/gui/fstoolsettingpanel.cpp \
    src/gui/fsmodulardoublespinbox.cpp \
    src/gui/fsloosespinbox.cpp

HEADERS += \
    src/core/fszip.h \
    src/core/fstoolmanager.h \
    src/core/fstool.h \
    src/core/fsthumbnail.h \
    src/core/fstabletinputdata.h \
    src/core/fstableteventfilter.h \
    src/core/fstabletevent.h \
    src/core/fsrasterlayer.h \
    src/core/fsrandomstring.h \
    src/core/fspalettemanager.h \
    src/core/fslayeredit.h \
    src/core/fslayer.h \
    src/core/fsgrouplayer.h \
    src/core/fsglobal.h \
    src/core/fsdrawutil.h \
    src/core/fsdocumentmodel.h \
    src/core/fscore.h \
    src/core/fscanvas.h \
    src/dialog/fsnewdocumentdialog.h \
    src/gui/fssimplebutton.h \
    src/gui/fspanel.h \
    src/gui/fslayertreeviewdelegate.h \
    src/gui/fslayertreepanel.h \
    src/gui/fsguimain.h \
    src/gui/fsdoubleslider.h \
    src/gui/fscolorwheel.h \
    src/gui/fscolorslider.h \
    src/gui/fscolorpanel.h \
    src/gui/fscolorbutton.h \
    src/gui/fscanvasview.h \
    src/gui/fsactionmanager.h \
    src/tool/fslayermovetool.h \
    src/tool/fsbrushtool.h \
    src/tool/fsbrushstroker.h \
    src/tool/fsbrushsetting.h \
    src/gui/fsaction.h \
    src/core/fslayerrenderer.h \
    src/core/fsscopedtimer.h \
    src/gui/fslayertreeview.h \
    src/gui/fsundoredoaction.h \
    src/core/fsdocumentcommand.h \
    src/dialog/fsexportdialog.h \
    src/gui/fswidgetgroup.h \
    src/gui/fstoolpanel.h \
    src/gui/fsborderwidget.h \
    src/tool/fsbrushsettingwidget.h \
    src/gui/fstoolsettingpanel.h \
    src/gui/fsmodulardoublespinbox.h \
    src/gui/fsloosespinbox.h
