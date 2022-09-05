# include <iostream>
# include <memory>
# include "../auto-generated/proto/cache.pb.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#define PORT 8080

int main() {
  std::cout<<"Client booting up!"<<std::endl;
  
  Request putRequest;
  putRequest.set_instruction("put");
  putRequest.set_key(17);
  putRequest.set_val(38);

  Request getRequest;
  getRequest.set_instruction("put");
  getRequest.set_key(17);
  getRequest.set_val(38);
  Request reqs[2] = {putRequest, getRequest};

  int sock = 0, valread, client_fd;
  struct sockaddr_in serv_addr;

  for (int i = 0; i < 2; i++) {
    Request req = reqs[i];
    int siz = req.ByteSize() + 4; // add 4 because we are going to add the size and an int is 4 bytes teeeheee!
    char *packet = new char[siz]; // this is the packet of data that weill be send across the network
    google::protobuf::io::ArrayOutputStream aos(packet, siz);
    google::protobuf::io::CodedOutputStream *codedOutput = new google::protobuf::io::CodedOutputStream(&aos);
    codedOutput->WriteVarint32(req.ByteSize());
    req.SerializeToCodedStream(codedOutput);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket creation"); 
      exit(EXIT_FAILURE);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) exit(EXIT_FAILURE);

    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("socket connection"); 
        exit(EXIT_FAILURE);
    }

    send(sock, packet, siz, 0);
    printf("Message sent\n");

    // valread = read(sock, buffer, 1024);
    delete packet;
    // closing the connected socket
    close(client_fd);
  }

  return 0;
}
