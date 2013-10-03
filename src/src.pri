
VERSION = 0.0.7

CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
	!contains(DEFINES, PF_ENABLE_DEBUG_OUTPUT) {
		DEFINES += QT_NO_DEBUG_OUTPUT
	}
}

mac {
	QMAKE_LFLAGS += -Wl,-install_name,@executable_path/../Frameworks/lib$${TARGET}.$${VERSION}.dylib
}

