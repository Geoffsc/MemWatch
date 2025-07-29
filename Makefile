CXX = g++
CXXFLAGS = -Wall -std=c++17
OBJS = meminfo.o
PREFIX ?= /usr/local

all: memwatch-gui

meminfo.o: src/meminfo.cpp src/meminfo.h
	$(CXX) $(CXXFLAGS) -c src/meminfo.cpp

memwatch-gui: src/memwatch_gui.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) src/memwatch_gui.cpp $(OBJS) -lfltk -lpthread -o memwatch-gui

clean:
	rm -f *.o memwatch-gui

install: memwatch-gui
	install -Dm755 memwatch-gui $(PREFIX)/bin/memwatch-gui
	install -Dm644 memwatch.desktop $(HOME)/.local/share/applications/memwatch.desktop
	@echo "Installed memwatch-gui to $(PREFIX)/bin"
