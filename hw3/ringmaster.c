// #include <iostream>
 #include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include "potato.h"
#include "helper.c"
int main(int argc, char *argv[]){
    if(argc != 4){
       return 0;
    }
    const char * port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    if(num_hops<0 || num_hops>512){
      return 0;
    }
    if(num_players <=0){
      return 0;
    }
    Potato potato;
    potato.numTurn = 0;
    potato.num_hops = num_hops;
    printf("Potato Ringmaster\n");
    printf("Players = %d\n",num_players);
    printf("Hops = %d\n",num_hops);
    int players[num_players];
    //char* playerIp[num_players];
    char** playerIp;
    //int playerPort[num_players];
    playerIp = malloc(num_players*sizeof(*playerIp));
    for(int i = 0;i<num_players;i++){
      playerIp[i] = malloc(30*sizeof(*playerIp[i]));
    }

    int masterfd = createServer(port);
    //connect with players
    for(int i = 0; i<num_players;i++){
        //char* playerOwnIp = malloc(30*sizeof(*playerOwnIp));
        //memset(playerOwnIp, 0, sizeof(playerOwnIp));
        memset(playerIp[i],0,sizeof(playerIp[i]));
        int playerfd = serverAccept(masterfd, &playerIp[i]);
        players[i] = playerfd;
        //playerIp[i] = playerOwnIp;
        //playerPort[i] = getPortNum(playerfd);
        send(playerfd,&i,sizeof(i),0);
        send(playerfd,&num_players,sizeof(num_players),0);
        //free(playerOwnIp);
        printf("Player %d is ready to play\n",i);
    }
    //send players information right neighbor's information & numplayers
    for(int i = 0;i<num_players-1;i++){
        send(players[i],&playerIp[i+1],sizeof(playerIp[i+1]),0);
    }
    send(players[num_players-1],&playerIp[0],sizeof(playerIp[0]),0);
    //game start
    srand((unsigned int)time(NULL) + num_players);
    int rdm = rand() % num_players;
    printf("Ready to start the game, sending potato to player %d\n", rdm);
    send(players[rdm],&potato,sizeof(potato),0);
    

    fd_set rdfds;
    int maxfd = players[0];
    FD_ZERO(&rdfds);
    for (int i = 0; i < num_players; i++) {
      FD_SET(players[i], &rdfds);
      maxfd = maxfd>players[i]?maxfd : players[i];
    }
    select(maxfd + 1, &rdfds, NULL, NULL, NULL);
    for (int i = 0; i < num_players; i++) {
      if (FD_ISSET(players[i], &rdfds)) {
        printf("receive from player %d\n",i);
        recv(players[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
      }
    }
    for(int i = 0; i<num_players;i++){
        send(players[i], &potato, sizeof(potato),0);
    }
    printf("Trace of potato:\n");
    for(int i = 0;i<potato.numTurn;i++){
      printf("%d,",potato.playerList[i]);
    }
    printf("%d\n",potato.playerList[potato.numTurn]);
    
    for(int i  = 0;i<num_players;i++){
      free(playerIp[i]);
    }
    free(playerIp);
    for(int i = 0; i<num_players;i++){
        close(players[i]);
    }
    close(masterfd);
    return 1;

}

