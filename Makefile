CXX = g++
VERSION = c++14

server:
	$(CXX) ./auto-generated/proto/cache.pb.cc  ./lru-cache/lru-cache.cpp ./server/server.cpp -o cachemoneyserver -std=$(VERSION) -Wall -lstdc++ -L/usr/lib -lprotobuf


client:
	$(CXX) ./auto-generated/proto/cache.pb.cc  ./client/client.cpp -o cachemoneyclient -lstdc++ -std=$(VERSION) -Wall -L/usr/lib -lprotobuf


clean:
	rm cachemoneyclient
	rm cachemoneyserver
