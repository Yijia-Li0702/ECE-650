// #ifndef POTATO_H
// #define POTATO_H
#include <stdio.h>
#include <string.h>
// #include <cstdio>
// #include <cstdlib>
// class Potato{
//     public:
//     int num_hops;
//     int playerList[512];

//     Potato(int num_hops):num_hops(num_hops){
//         memset(playerList,0,sizeof(playerList);
//     }
// };
struct potato_t{
  int num_hops;
  int playerList[512];
  int numTurn;
};
typedef struct potato_t Potato;
// #endif