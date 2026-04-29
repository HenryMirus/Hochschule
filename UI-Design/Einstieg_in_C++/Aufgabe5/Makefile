QMAKE = C:/Qt/6.11.0/mingw_64/bin/qmake.exe
QTCXX = C:/Qt/Tools/mingw1310_64/bin/g++.exe
QTCC = C:/Qt/Tools/mingw1310_64/bin/gcc.exe
QTMAKE = C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
PROJECT = Aufgabe5.pro
GEN_MAKEFILE = Makefile.qt

all: main

main: $(GEN_MAKEFILE)
	$(QTMAKE) -f $(GEN_MAKEFILE)

$(GEN_MAKEFILE): $(PROJECT) main.cpp
	$(QMAKE) $(PROJECT) -o $(GEN_MAKEFILE) QMAKE_CXX=$(QTCXX) QMAKE_CC=$(QTCC) QMAKE_LINK=$(QTCXX)

clean:
	cmd /C if exist $(GEN_MAKEFILE) $(QTMAKE) -f $(GEN_MAKEFILE) clean
	cmd /C if exist main.exe del /Q main.exe
	cmd /C if exist main del /Q main
	cmd /C if exist main.o del /Q main.o
	cmd /C if exist moc_main.cpp del /Q moc_main.cpp
	cmd /C if exist .qmake.stash del /Q .qmake.stash

.PHONY: all clean