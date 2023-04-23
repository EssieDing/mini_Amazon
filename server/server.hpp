#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <sstream>
#include <random>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <memory>
#include <cstdlib>
#include "GPB_message.hpp"
#include "protobuf/world_amazon.pb.h"
#include "protobuf/amazon_ups.pb.h"


typedef struct warehouse{
    int id;
    int x;
    int y;
    warehouse(int id,int x,int y):id(id),x(x),y(y){}
}Warehouse;


int Amazon_connect_to_world(int port=23456,Warehouse w,int& world_id){
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  
    serv_addr.sin_port = htons(23456); //port for Amazon

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))!=0){
        std::cout<<"Amazon: connect world failed"<<std::endl;
        return -1;
    } 

    //create a new world
    AConnect aconnect;
    auto ainitwarehouse = aconnect.add_initwh();
    ainitwarehouse->set_id(w.id);
    ainitwarehouse->set_x(w.x);
    ainitwarehouse->set_y(w.y);
    aconnect.set_isamazon(true);

    //send to world
    std::unique_ptr<GPBFileOutputStream> output(new GPBFileOutputStream(sock));
    if(sendMesgTo(aconnect,output.get())!=true){
        std::cout<<"Amazon: send init world failed"<<std::endl;
        return -1;
    }
    //receive from world
    std::unique_ptr<GPBFileInputStream> input(new GPBFileInputStream(sock));
    AConnected aconnected;
    if(recvMesgFrom(aconnected,input.get())!=true){
        std::cout<<"Amazon: receive message form world failed"<<std::endl;
        return -1;
    }
    std::string result = aconnected.result();
    if(result.find("connected!")==std::string::npos){
        std::cout<<"Amazon: connect world failed"<<std::endl;
        std::cout<<"Amazon: "<<result<<std::endl;
        return -1;
    }

    return sock;
}

int Amazon_wait_for_UPS(int port=5688,int& world_id){
    struct sockaddr_in serv_addr;
    int Connect;
    int Server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(5688);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    bind(Server, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
    listen(Server, SOMAXCONN);

    sockaddr_in ClientAddr;
    socklen_t client_len = sizeof(ClientAddr);
    Connect = accept(Server, (sockaddr*)&ClientAddr, &client_len);

    AUInitConnect auinitconnect;
    auinitconnect.set_worldid(world_id);
    auinitconnect.set_seqnum(Connect);
    std::unique_ptr<GPBFileOutputStream> output(new GPBFileOutputStream(Connect));
    if(sendMesgTo(auinitconnect,output.get())!=true){
        std::cout<<"Amazon: send connection to UPS failed"<<std::endl;
        return -1;
    }

    UAConfirmConnected uaconfrimconnected;
    std::unique_ptr<GPBFileInputStream> input(new GPBFileInputStream(Connect));
    if(recvMesgFrom(uaconfrimconnected,input.get())!=true){
        std::cout<<"Amazon: receive from UPS failed"<<std::endl;
        return -1;
    }
    if(uaconfrimconnected.connected()==false){
        std::cout<<"Amazon: UPS connected to world failed"<<std::endl;
        return -1;
    }

    return Connect;
}

#endif