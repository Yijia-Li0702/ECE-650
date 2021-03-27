// server code
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

int createServer(const char* port){
  // std::cout <<"createServer"<<std::endl;
     int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo * host_info_list;
    const char *hostname = NULL;
    //const char *port     = "4444";

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if
  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if

  freeaddrinfo(host_info_list);

  return socket_fd;
}

int createClient(const char* port, const char * hostname){
    int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;


  memset(&host_info, 0, sizeof(host_info)); //clean
  host_info.ai_family   = AF_UNSPEC; //not choosing IPv4/v6
  host_info.ai_socktype = SOCK_STREAM; //reliable coomunication

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if
  
  // cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return EXIT_FAILURE;
  } //if

  freeaddrinfo(host_info_list);

  return socket_fd;
}

int serverAccept(int socket_fd, std::string * ip){
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  //  printf("before accept\n");
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  // printf("accpet %d\n", client_connection_fd);
  struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;//change to ipv4
  if(client_connection_fd == -1){
    // cerr<<"Error: cannot accept connection on socket" <<endl;
    // printf("serverAccept fail\n");
    return -1;
    //exit(EXIT_FAILURE);
  }

  *ip = inet_ntoa(addr->sin_addr);
  
  return client_connection_fd;
}


