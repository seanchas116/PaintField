
include(../src.pri)

PF_VERSION = 0.0.4

macx {
	PF_PLATFORM = "mac"
}

unix:!macx {
	PF_PLATFORM = "unix"
}

windows {
	PF_PLATFORM = "windows"
}

QT += core gui network xml svg
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -std=c++11

contains(DEFINES, PF_TEST) {
	QT += testlib
}

contains(QMAKE_CXX, clang++) {
	QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-private-field
	mac {
		QMAKE_CXXFLAGS += -stdlib=libc++
		QMAKE_LFLAGS += -stdlib=libc++
	}
	DEFINES += Q_COMPILER_INITIALIZER_LISTS
}

INCLUDEPATH += $$PWD/.. $$PWD/../libs $$PWD/../libs/Malachite/include

#DEFINES += PF_FORCE_RASTER_ENGINE

# defining a function that returns the relative path from 2 paths
# note that the head of a result is always '/'
defineReplace(relativePathFrom) {
	
	path_to = $$1
	path_from = $$2
	
	path_to_from = $$replace(path_to, ^$${path_from}, )
	
	contains(path_to_from, ^$${path_to}$) {
		path_to_from = $$replace(path_from, ^$$path_to, )
		path_to_from = $$replace(path_to_from, [^/]+, ..)
	}
	
	return($$path_to_from)
}

PF_OUT_PWD = $$OUT_PWD$$relativePathFrom($$PWD, $$_PRO_FILE_PWD_)

win32 {
	CONFIG(debug, debug|release) {
		PF_OUT_SUBDIR = debug
	} else {
		PF_OUT_SUBDIR = release
	}
} else {
	PF_OUT_SUBDIR =
}

LIBS += -L$$PF_OUT_PWD/../libs/Malachite/src/$$PF_OUT_SUBDIR
LIBS += -L$$PF_OUT_PWD/../libs/minizip/$$PF_OUT_SUBDIR
LIBS += -L$$PF_OUT_PWD/../libs/qtsingleapplication/$$PF_OUT_SUBDIR
LIBS += -lfreeimage -lmalachite -lpaintfield-minizip -lpaintfield-qtsingleapplication
