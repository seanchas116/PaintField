
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -std=c++11

mac {
	QMAKE_OBJECTIVE_CFLAGS += -std=c++11
}

cxx_clang = $$find(QMAKE_CXX, "clang")

!isEmpty(cxx_clang) {
	
	message("using clang")
	
}
