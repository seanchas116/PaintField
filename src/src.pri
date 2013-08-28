
VERSION = 0.0.6

CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
	!contains(DEFINES, PF_DEBUG_OUTPUT) {
		DEFINES += QT_NO_DEBUG_OUTPUT
	}
}

contains(QMAKE_CXX, clang++) {
  QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-private-field
}

mac {
	QMAKE_LFLAGS += -Wl,-install_name,@executable_path/../Frameworks/lib$${TARGET}.$${VERSION}.dylib
	QMAKE_CFLAGS_X86_64 = -mmacosx-version-min=10.7
	QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
	QMAKE_OBJECTIVE_CFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
	QMAKE_LFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}

