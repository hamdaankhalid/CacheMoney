#include <iostream>
#include <thread>
#include <memory>

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <unistd.h> // For read

#include "./lru-cache.h"

#define SERVER_PORT 8080

// void testThread(std::unique_ptr<LRUCache> &lruc) {
//   lruc->put(2, 5);
//   lruc->get(2);
//   lruc->put(2, 2);
//   lruc->get(2);
//   lruc->get(2);
//   lruc->put(4, 3);
//   lruc->get(2);
//   lruc->get(4);
//   lruc->get(2);
//   lruc->get(4);
//   lruc->get(69);
// }

int main() {
  int opt = 1;
  int socketFd = ::socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == socketFd) {
    perror("server");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080 
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
  { 
    perror("setsockopt"); 
    exit(EXIT_FAILURE); 
  }

  struct sockaddr_in sockaddr, cliaddr;
  
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;

  // htons is necessary to convert a number to network byte order
  sockaddr.sin_port = htons(SERVER_PORT);
  
  auto bindSuccess = ::bind(socketFd, (struct sockaddr*) &sockaddr, sizeof(sockaddr));

  if (bindSuccess < 0) {
    perror("bind failed"); 
    exit(EXIT_FAILURE);
  }

  int listenDescriptor = listen(socketFd, 10);
  if (listenDescriptor != 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::unique_ptr<LRUCache> lruc = std::make_unique<LRUCache>(2);

  // testThread(lruc);
  
  auto addrlen = sizeof(cliaddr);
  int connection;
  
  std::cout << "Server is starting on port " << SERVER_PORT << std::endl;
  
  while (true) {
    connection = accept(socketFd, (struct sockaddr*) &cliaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      perror("connection accept"); 
      exit(EXIT_FAILURE);
    }

    // Read from the connection
    char buffer[100];
    read(connection, buffer, 100);
    std::cout << "The message was: " << buffer << std::endl;

    // message can be put or get
    close(connection);
  }
  
  close(listenDescriptor);
  shutdown(socketFd, SHUT_RDWR);
  
  return 0;
}
