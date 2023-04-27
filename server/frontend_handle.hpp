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
    std::cout<<"Amazon: process order from front end"<<std::endl;
    std::vector<AProduct> products;
    tinyxml2::XMLElement *product_list = doc.RootElement()->FirstChildElement("items");
    tinyxml2::XMLElement *product = product_list->FirstChildElement();
    while(product!=nullptr){
        AProduct aproduct;
        aproduct.set_id(std::stoll(product->FirstChildElement("id")->GetText()));
        aproduct.set_description(product->FirstChildElement("description")->GetText());
        aproduct.set_count(std::stoi(product->FirstChildElement("count")->GetText()));
        products.push_back(aproduct);
        product=product->NextSiblingElement();
    }
    std::string accountname = doc.RootElement()->FirstChildElement("account_name")->GetText();
    long long order_id=std::stoll(doc.RootElement()->FirstChildElement("order_id")->GetText());
    int addr_x=std::stoi(doc.RootElement()->FirstChildElement("addr_x")->GetText());
    int addr_y=std::stoi(doc.RootElement()->FirstChildElement("addr_y")->GetText());
    // send_ApurchaseMore_to_world
    // TO-DO: Change warehouse id
    send_ApurchaseMore_to_world(1,products,order_id,accountname,addr_x,addr_y);
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