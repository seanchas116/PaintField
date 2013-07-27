
include(malachite.pri)

INCLUDEPATH += ../include
LIBS += -L$$OUT_PWD/../src -lmalachite

mac {
	SHAREDLIB.files = $$OUT_PWD/../src/libmalachite.$${VERSION}.dylib
	SHAREDLIB.path = Contents/Frameworks
	QMAKE_BUNDLE_DATA += SHAREDLIB
}
