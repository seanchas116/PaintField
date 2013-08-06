
include(paintfield.pri)

mac {
	CONFIG += lib_bundle

	SHAREDLIB.files += $$OUT_PWD/../../libs/minizip/libpaintfield-minizip.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../../libs/qtsingleapplication/libpaintfield-qtsingleapplication.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../../libs/Malachite/src/libmalachite.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../core/libpaintfield-core.$${VERSION}.dylib
	SHAREDLIB.files += $$OUT_PWD/../extensions/libpaintfield-extensions.$${VERSION}.dylib

	SHAREDLIB.path = Contents/Frameworks
	QMAKE_BUNDLE_DATA += SHAREDLIB

	QMAKE_POST_LINK = "ruby $${PWD}/../../scripts/copy_libs.rb $${PWD} $${OUT_PWD} $${TARGET} $${VERSION}"
	
}

LIBS += -L$$PF_OUT_PWD/extensions/$$PF_OUT_SUBDIR
LIBS += -lpaintfield-extensions

unix:!macx {
  QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}
