main: main.o fak.o
	g++ -Wall -g main.o fak.o -o main

main.o: main.cpp fak.h
	g++ -Wall -g -c main.cpp -o main.o

fak.o: fak.cpp fak.h
	g++ -Wall -g -c fak.cpp -o fak.o

clean:
	cmd /C if exist main.exe del /Q main.exe
	cmd /C if exist main del /Q main
	cmd /C if exist main.o del /Q main.o
	cmd /C if exist fak.o del /Q fak.o