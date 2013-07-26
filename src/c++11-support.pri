
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -std=c++11

mac {
  QMAKE_OBJECTIVE_CFLAGS += -std=c++11
}

contains(QMAKE_CXX, clang++) {
  mac {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    QMAKE_OBJECTIVE_CFLAGS += -stdlib=libc++
  }
  DEFINES += Q_COMPILER_INITIALIZER_LISTS Q_COMPILER_RVALUE_REFS
}
