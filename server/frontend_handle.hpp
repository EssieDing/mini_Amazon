#ifndef FRONTEND_HANDLE_HPP
#define FRONTEND_HANDLE_HPP
#include<iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<mutex>
#include<vector>
#include<thread>
#include<memory>
#include<cstdlib>
#include<exception>
#include <unistd.h>
#include "common.hpp"
#include "world_handle.hpp"
#include "tinyxml2.h"



int Process_order(tinyxml2::XMLDocument& doc ){

    // send_ApurchaseMore_to_world
    return 0;   
}


int receive_data_from_frontend(int Connect){
    try{
        std::vector<char> buf(10000);
        int bytes = recv(Connect, buf.data(), buf.size(), 0);
    
        //read request length
        std::string order(buf.data(), bytes);
        while(true){
            std::vector<char> buf(10000);
            int bytes = recv(Connect, buf.data(), buf.size(), 0);
            order.append(buf.data(),bytes);
            if(bytes<=0)
                break;
        }
        std::cout<<"Order received: "<<order<<std::endl;
        tinyxml2::XMLDocument doc;
        doc.Parse(order.c_str());
        Process_order(doc);
    }
    catch(std::exception& e){
        std::cout<<"Amazon: receive data from frontend failed"<<std::endl;
        return -1;
    }

    close(Connect);
    return 0;
}

int Amazon_connect_frontend(int port){
    struct sockaddr_in serv_addr;
    int Connect;
    int Server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    bind(Server, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
    listen(Server, SOMAXCONN);

    while(true){
        sockaddr_in ClientAddr;
        socklen_t client_len = sizeof(ClientAddr);
        Connect = accept(Server, (sockaddr*)&ClientAddr, &client_len);
        pool.enqueue(receive_data_from_frontend,Connect);
    }

}


class FrontendHandler {
public:
    FrontendHandler() {}
    void operator()() {
        Amazon_connect_frontend(front_end_port);
    }    
};
#endif 