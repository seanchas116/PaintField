
VERSION = 0.0.6

CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
}

contains(QMAKE_CXX, clang++) {
  QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-private-field
}

mac {
	QMAKE_LFLAGS += -Wl,-install_name,@executable_path/../Frameworks/lib$${TARGET}.$${VERSION}.dylib
}

