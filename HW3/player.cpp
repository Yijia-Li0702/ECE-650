// #include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <stdlib.h> 
#include <stdio.h>
#include <time.h>
#include "potato.h"
#include "helper.c"

int main(int argc, char ** argv){
    if(argc!=3){
        return 0;
    }
    const char * machine_hostname = argv[1];
    const char* master_port_num = argv[2];
    int masterfd = createClient(master_port_num,machine_hostname);
    //int neighborPort;
    int id;
    int num_players;
    char neighborIp[50];
    recv(masterfd,&id,sizeof(id),0);
    recv(masterfd,&num_players,sizeof(num_players),0);
    printf("Connected as player %d out of %d total players\n",id,num_players);
    recv(masterfd,&neighborIp,sizeof(neighborIp),MSG_WAITALL);
    char myPort[50];
    sprintf(myPort,"%d",5000+id);
    int myfd = createServer(myPort);
    //connect with right neighbor
    int neighborPort;
    if(id == num_players-1){
        neighborPort = 5000;
    } else{
        neighborPort = 5000+id+1;
    }
     printf("neighborPort %d\n",neighborPort);
    char convertedNeighborPort[50];
    sprintf(convertedNeighborPort,"%d",neighborPort);
    printf("neighborIp %s\n",neighborIp);
    int rightNeighborfd = createClient(convertedNeighborPort,neighborIp);
    printf("connect with right player %d\n",rightNeighborfd);
    //connect with left neighbor

    char * leftNeighborIp;
    int leftNeighborfd = serverAccept(myfd,&leftNeighborIp);
    printf("connect with left player %d\n",leftNeighborfd);
    //game begin ready to receive potato
    Potato potato;
    srand((unsigned int)time(NULL)+id);
    fd_set rdfds;
    int maxfdmid = masterfd>leftNeighborfd?masterfd:leftNeighborfd;
    int maxfd = maxfdmid>rightNeighborfd?maxfdmid:rightNeighborfd;

    while(1){
        FD_ZERO(&rdfds);
        FD_SET(masterfd, &rdfds);
        FD_SET(leftNeighborfd, &rdfds);
        FD_SET(rightNeighborfd, &rdfds);
        select(maxfd + 1, &rdfds, NULL, NULL, NULL);
        if (FD_ISSET(masterfd, &rdfds)) {
            printf("from master\n");
            recv(masterfd, &potato, sizeof(potato), MSG_WAITALL);
        }
        if(FD_ISSET(leftNeighborfd, &rdfds)){
            printf("from leftNeighborfd\n");
            recv(leftNeighborfd,&potato, sizeof(potato), MSG_WAITALL);
        }
        if(FD_ISSET(rightNeighborfd, &rdfds)){
            printf("from rightNeighborfd\n");
            recv(rightNeighborfd,&potato, sizeof(potato), MSG_WAITALL);
        }
        if(potato.num_hops == 0){
            break;
        }
        potato.num_hops--;
        potato.playerList[potato.numTurn] = id;
        int rdm = rand() % 2;
        
        if(potato.num_hops == 0){
            send(masterfd, &potato, sizeof(potato), 0);
            printf("I’m it\n");
        } else if(rdm == 0){
            potato.numTurn++;
            send(leftNeighborfd,&potato, sizeof(potato), 0);
            printf("Sending potato to %d\n",rdm);            
        } else{
            potato.numTurn++;
            send(rightNeighborfd,&potato, sizeof(potato), 0);
            printf("Sending potato to %d\n",rdm);            
        }
        
    }
    FD_ZERO(&rdfds);
    close(masterfd);
    close(leftNeighborfd);
    close(rightNeighborfd);
    close(myfd);
    return 1;
}
    
    



