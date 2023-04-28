#include"ThreadPool.h"
#include"server.hpp"
#include<sys/socket.h>
#include <unistd.h> 
#include<arpa/inet.h> 
#include<thread>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<thread>
#include <pqxx/pqxx>



int main(int argc, char *argv[]) {

    Init_warehouse();
    memset(&send_acks,0,sizeof(send_acks));
    memset(&recv_acks,0,sizeof(recv_acks));

    int world_id;
    for(;;){
        if((world_sock=Amazon_connect_to_world(23456,world_id))==-1){
            std::cout<<"Amazon: Failed to connect to world"<<std::endl;
            sleep(1);
            continue;
        }
        else{
            std::cout<<"Amazon: Connected to world"<<std::endl;
            break;
        }
        // if((ups_sock=Amazon_wait_for_UPS(5688,world_id))==-1){
        //     std::cout<<"Amazon: Failed to connect to UPS"<<std::endl;
        //     sleep(1);
        //     continue;
        // }
        // else{
        //     std::cout<<"Amazon: Connected to UPS"<<std::endl;
        // }
        

    }
    pool.enqueue(WorldHandler());
    // pool.enqueue(UPSHandler());
   // pool.enqueue(FrontendHandler());
    pool.enqueue([](){
        for(;;){
            test_send_Apurchasemore();
        }
    });
    return 0;
}


