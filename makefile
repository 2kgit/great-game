CC = g++
CFLAGS = -std=c++0x -lGL -lglfw

all: game.o

game.o: source.cpp
	echo ">> Preparing.."
	mkdir -p bin >> /dev/null
	echo ">> Compiling..."
	g++ source.cpp $(CFLAGS) -o bin/game > /dev/null
	echo ">> Make program executable..."
	chmod +x bin/game > /dev/null

run:
	sh bin/game
