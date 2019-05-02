all:
	g++ code/shader.cpp -c -o code/shader.o
	g++ code/texture.cpp -c -o code/texture.o
	g++ code/controls.cpp -c -o code/controls.o
	g++ code/objloader.cpp -c -o code/objloader.o
	g++ `pkg-config --cflags glfw3` -ggdb3 -Wall -std=c++11 -o code/museum code/shader.o code/texture.o code/controls.o code/objloader.o code/main.cpp -lGL -lSOIL -lGLEW -lglut -lGLU `pkg-config --libs glfw3`

run:
	./code/museum

clean:
	rm code/*.o
	rm code/museum
