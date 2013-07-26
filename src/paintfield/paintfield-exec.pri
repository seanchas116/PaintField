
include(paintfield.pri)

mac {
	SHAREDLIB.files += $$OUT_PWD/../../libs/minizip/libpaintfield-minizip.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../../libs/qtsingleapplication/libpaintfield-qtsingleapplication.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../../libs/Malachite/src/libmalachite.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../core/libpaintfield-core.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../extensions/libpaintfield-extensions.$${VERSION}.dylib

	SHAREDLIB.path = Contents/Frameworks
	QMAKE_BUNDLE_DATA += SHAREDLIB
}

LIBS += -L$$PF_OUT_PWD/extensions/$$PF_OUT_SUBDIR
LIBS += -lpaintfield-extensions

unix:!macx {
  QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}
