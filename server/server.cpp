#include <iostream>
#include <thread>
#include <memory>
#include <iterator>

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <unistd.h> // For read
#include "../lru-cache/lru-cache.h"
#include "../auto-generated/proto/cache.pb.h"

#define SERVER_PORT 8080

google::protobuf::uint32 readHdr(char *buf)
{
  google::protobuf::uint32 size;
  google::protobuf::io::ArrayInputStream ais(buf,4);
  google::protobuf::io::CodedInputStream coded_input(&ais);
  coded_input.ReadVarint32(&size); // Decode the HDR and get the size
  return size;
}

void handleBody(int connection, google::protobuf::uint32 siz,  std::shared_ptr<LRUCache> lruc) {
  int bytecount;
  char buffer [siz+4];//size of the payload and hdr
  //Read the entire buffer including the hdr
  if((bytecount = recv(connection, (void *)buffer, 4+siz, MSG_WAITALL))== -1){
    fprintf(stderr, "Error receiving data %d\n", errno);
  }
  
  std::string stringifiedBuf = std::string(buffer);
  // The buffer is stringified so it is passed by value! This slows it down but it also eliminates the sharing of buffer across threads
  std::thread _threadHandler([](int connection, int bytecount, std::string stringifiedBuf, google::protobuf::uint32 siz, std::shared_ptr<LRUCache> lruc){
    char* buf;
    buf = &stringifiedBuf[0];
    Request payload;
    std::cout<<"Second read byte count is "<<bytecount<<std::endl;
    //Assign ArrayInputStream with enough memory 
    google::protobuf::io::ArrayInputStream ais(buf, siz+4);
    google::protobuf::io::CodedInputStream coded_input(&ais);
    //Read an unsigned integer with Varint encoding, truncating to 32 bits.
    coded_input.ReadVarint32(&siz);
    //After the message's length is read, PushLimit() is used to prevent the CodedInputStream 
    //from reading beyond that length.Limits are used when parsing length-delimited 
    //embedded messages
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    //De-Serialize
    payload.ParseFromCodedStream(&coded_input);
    //Once the embedded message has been parsed, PopLimit() is called to undo the limit
    coded_input.PopLimit(msgLimit);
    std::cout<<"Message is "<<payload.DebugString();

    if (payload.instruction() == "get") {
      lruc->get(payload.key());
    } else {
      lruc->put(payload.key(), payload.val());
    }
    close(connection);
  }, connection, bytecount, stringifiedBuf, siz, lruc);
  _threadHandler.detach();
}

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

  std::shared_ptr<LRUCache> lruc = std::make_shared<LRUCache> (10);
  auto addrlen = sizeof(cliaddr);
  int connection;
  std::cout << "Server is starting on port " << SERVER_PORT << std::endl;
  
  while (true) {

    connection = accept(socketFd, (struct sockaddr*) &cliaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      perror("connection accept"); 
      exit(EXIT_FAILURE);
    }
    
    // single threaded server below commented out
    char buffer[4];
    memset(buffer, '\0', 4);
    google::protobuf::uint32 size;
    recv(connection, buffer, 4, MSG_PEEK);
    size = readHdr((char *) buffer);
    handleBody(connection, size, lruc);
  }

  close(listenDescriptor);
  shutdown(socketFd, SHUT_RDWR);

  return 0;
}
