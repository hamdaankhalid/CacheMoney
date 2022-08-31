CXX = g++
VERSION = c++14

server:
	$(CXX) src/main.cpp src/lru-cache.cpp -o meowcache -lstdc++ -std=$(VERSION) -Wall

clean:
	rm cachemoney
