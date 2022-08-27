CXX = g++
VERSION = c++11

server:
	$(CXX) src/main.cpp src/lru-cache.cpp -o meowcache -lstdc++ -std=$(VERSION) -Wall

clean:
	rm meowcache
