######################################################################
# Automatically generated by qmake (2.01a) ? 10? 28 13:02:30 2012
######################################################################

TEMPLATE = lib
TARGET = paintfield-core

DEFINES += PAINTFIELD_LIBRARY

include(../paintfield-core.pri)
include(../postlink.pri)

# Input
HEADERS += \
           document.h \
           documentio.h \
           drawutil.h \
           global.h \
           layer.h \
           layeredit.h \
           layermodel.h \
           layerrenderer.h \
           palettemanager.h \
           randomstring.h \
           tabletevent.h \
           tabletinputdata.h \
           thumbnail.h \
           tool.h \
           toolmanager.h \
           util.h \
           workspacemanager.h \
           workspaceview.h \
           dialogs/exportdialog.h \
           dialogs/newdocumentdialog.h \
           internal/layermodelcommand.h \
           widgets/colorbutton.h \
           widgets/colorslider.h \
           widgets/colorwheel.h \
           widgets/doubleslider.h \
           widgets/loosespinbox.h \
           widgets/modulardoublespinbox.h \
           widgets/simplebutton.h \
           widgets/widgetgroup.h \
    dialogs/messagebox.h \
    widgets/docktabwidget.h \
    canvasview.h \
    smartpointer.h \
    interface.h \
    appcontroller.h \
    application.h \
    internal/applicationeventfilter.h \
    widgets/vanishingscrollbar.h \
    widgets/docktabmotherwidget.h \
    signalconverter.h \
    callbackanimation.h \
    librarymodel.h \
    keytracker.h \
    dialogs/filedialog.h \
    canvastabwidget.h \
    canvassplitwidget.h \
    canvassplitareacontroller.h \
    widgets/memorizablesplitter.h \
    settingsmanager.h \
    workspacetabwidget.h \
    scopedtimer.h \
    canvas.h \
    workspace.h \
    extension.h \
    extensionmanager.h \
    canvasviewwrapper.h \
    canvasviewport.h \
    selection.h \
    tabletpointerinfo.h \
    cursorstack.h \
    generaleditaction.h \
    proxyaction.h \
    rasterlayer.h \
    grouplayer.h \
    shapelayer.h \
    layerfactorymanager.h \
    documentcontroller.h \
    documentreferencemanager.h \
    rectlayer.h \
    unittest/test_shapelayer.h \
    unittest/test_rectlayer.h \
    unittest/test_librarymodel.h \
    unittest/test_documentio.h \
    unittest/test_document.h \
    unittest/autotest.h \
    unittest/testutil.h
FORMS += dialogs/exportdialog.ui dialogs/newdocumentdialog.ui
SOURCES += \
           document.cpp \
           documentio.cpp \
           drawutil.cpp \
           layer.cpp \
           layeredit.cpp \
           layermodel.cpp \
           layerrenderer.cpp \
           palettemanager.cpp \
           randomstring.cpp \
           thumbnail.cpp \
           toolmanager.cpp \
           util.cpp \
           workspacemanager.cpp \
           workspaceview.cpp \
           dialogs/exportdialog.cpp \
           dialogs/newdocumentdialog.cpp \
           internal/layermodelcommand.cpp \
           widgets/colorbutton.cpp \
           widgets/colorslider.cpp \
           widgets/colorwheel.cpp \
           widgets/doubleslider.cpp \
           widgets/loosespinbox.cpp \
           widgets/modulardoublespinbox.cpp \
           widgets/simplebutton.cpp \
           widgets/widgetgroup.cpp \
    dialogs/messagebox.cpp \
    widgets/docktabwidget.cpp \
    widgets/docktabmotherwidget.cpp \
    canvasview.cpp \
    tool.cpp \
    appcontroller.cpp \
    application.cpp \
    internal/applicationeventfilter.cpp \
    widgets/vanishingscrollbar.cpp \
    signalconverter.cpp \
    callbackanimation.cpp \
    librarymodel.cpp \
    keytracker.cpp \
    dialogs/filedialog.cpp \
    canvastabwidget.cpp \
    canvassplitwidget.cpp \
    canvassplitareacontroller.cpp \
    widgets/memorizablesplitter.cpp \
    settingsmanager.cpp \
    workspacetabwidget.cpp \
    canvas.cpp \
    workspace.cpp \
    extension.cpp \
    extensionmanager.cpp \
    canvasviewwrapper.cpp \
    canvasviewport.cpp \
    selection.cpp \
    cursorstack.cpp \
    generaleditaction.cpp \
    proxyaction.cpp \
    rasterlayer.cpp \
    grouplayer.cpp \
    shapelayer.cpp \
    layerfactorymanager.cpp \
    documentcontroller.cpp \
    documentreferencemanager.cpp \
    rectlayer.cpp \
    unittest/test_shapelayer.cpp \
    unittest/test_rectlayer.cpp \
    unittest/test_librarymodel.cpp \
    unittest/test_documentio.cpp \
    unittest/test_document.cpp \
    unittest/autotest.cpp \
    unittest/testutil.cpp

RESOURCES += \
    resources/resource-paintfield-core.qrc
