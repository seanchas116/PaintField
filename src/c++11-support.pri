
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -std=c++11

mac {
	QMAKE_OBJECTIVE_CFLAGS += -std=c++11
}

cxx_clang = $$find(QMAKE_CXX, "clang")

!isEmpty(cxx_clang) {
	
	#message("using clang")
	
	mac {
		QMAKE_CXXFLAGS += -stdlib=libc++
		QMAKE_LFLAGS += -stdlib=libc++
		QMAKE_OBJECTIVE_CFLAGS += -stdlib=libc++
	}
	DEFINES += Q_COMPILER_INITIALIZER_LISTS Q_COMPILER_RVALUE_REFS
}
