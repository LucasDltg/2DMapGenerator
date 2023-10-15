

main : main.o Perlin.o
	g++ main.o Perlin.o -o main -lSDL2

main.o : main.cpp
	g++ main.cpp -o main.o -c

Perlin.o : Perlin.cpp
	g++ Perlin.cpp -o Perlin.o -c

clean:
	rm main
	rm *.o