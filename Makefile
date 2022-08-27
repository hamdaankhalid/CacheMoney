CXX = g++
VERSION = c++11

all:
	$(CXX) src/main.cpp src/lru-cache.cpp -o meowcache -lstdc++ -std=$(VERSION)

clean:
	rm meowcache
