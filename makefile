capture: capture.cpp
	g++ -std=c++11 capture.cpp -o capture `pkg-config --cflags --libs opencv`
example: example.cpp
	g++ -std=c++11 example.cpp -o example `pkg-config --cflags --libs opencv`