CONFIG(debug, debug|release) {
	DEFINES += QT_DEBUG
} else {
	DEFINES += QT_NO_DEBUG
}

mac {
	QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
	QMAKE_CFLAGS_X86_64 = -mmacosx-version-min=10.7
	QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
	QMAKE_OBJECTIVE_CFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
	QMAKE_LFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}

