CXX := clang++
CFLAGS := -O3 -std=c++20 -g
INCLUDE := -Iinclude
PYINCLUDE := -I/usr/include/python3.10
BOOST := -lpython3.10 -lboost_python3 -lpthread

LIB_SRC := src/wordle.cpp
WRAPPER := src/py_wrapper.cpp
LIB := lib/libwordle.so
PYLIB := lib/pywordle.so


.PHONY: all clean pywordle

all: pywordle

pywordle: $(PYLIB)

$(PYLIB): $(LIB) $(LIB_SRC) $(WRAPPER)
	$(CXX) $(CFLAGS) -shared $(INCLUDE) $(PYINCLUDE) -fPIC $(WRAPPER) $(LIB) -o $(PYLIB) $(BOOST)

$(LIB): $(LIB_SRC)
	$(CXX) $(CFLAGS) -shared $(INCLUDE) -fPIC $(LIB_SRC) -o $(LIB)