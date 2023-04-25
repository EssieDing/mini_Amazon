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


class WorldHandler {
public:
    WorldHandler() {}
    void operator()(int Connect, sockaddr_in ClientAddr) {
        // std::cout<<"Client connected!"<<std::endl;
        // pqxx::connection *C;
        // try{
        //     C = new pqxx::connection("dbname=trades user=postgres password=passw0rd host=db port=5432");
        //     //C = new pqxx::connection("dbname=trades user=postgres password=passw0rd");
            
        //     if(C->is_open()){
        //         std::cout<<"Connected to database successfully: "<< C->dbname()<<std::endl;
        //     }else{
        //         std::cerr<<"Can't open database"<<std::endl;
        //         exit(EXIT_FAILURE);
        //     }
        // }catch(const std::exception &e){
        //     std::cerr<<e.what()<<std::endl;
        //     exit(EXIT_FAILURE);
        // }
        
        
        // //read request 
        std::vector<char> buf(10000);
        int bytes = recv(Connect, buf.data(), buf.size(), 0);
        
        //read request length
        std::string raw_str(buf.data(), bytes);
        // std::cout<<"Request raw request: "<<raw_str<<std::endl;
        size_t first_line_end = raw_str.find("\n");
        if (first_line_end == std::string::npos) {
            std::cerr << "Invalid request!" << std::endl;
            return;
        }
        int request_length = std::stoi(raw_str.substr(0, first_line_end));
        std::cout<<"Request length: "<<request_length<<std::endl;

        size_t request_start = first_line_end + 1;
        std::string request=raw_str.substr(request_start);
        int total_bytes=request.length();
        while(request_length>=total_bytes){
            std::vector<char> buf(10000);
            int bytes = recv(Connect, buf.data(), buf.size(), 0);
            request.append(buf.data(),bytes);
            if(bytes>0){
                total_bytes+=bytes;
            }
            else
                break;
        }
      
    }    
};

class UPSHandler {
public:
    UPSHandler() {}
    void operator()(int Connect, sockaddr_in ClientAddr) {
                // //read request 
        std::vector<char> buf(10000);
        int bytes = recv(Connect, buf.data(), buf.size(), 0);
    }
};


pqxx::connection * connectDB() {
    pqxx::connection *C;
    try{
        C = new pqxx::connection("dbname=db user=postgres password=passw0rd hostaddr=127.0.0.1 port=5432");
            
        if(C->is_open()){
            std::cout<<"Connected to database successfully: "<< C->dbname()<<std::endl;
        }else{
            std::cerr<<"Can't open database"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }catch(const std::exception &e){
        std::cerr<<e.what()<<std::endl;
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]) {
    
    // pqxx::connection *C;
    // C = new pqxx::connection("dbname=trades user=postgres password=passw0rd host=db port=5432");
    // C = new pqxx::connection("dbname=trades user=postgres password=passw0rd");
    // DropTable(C);
    // CreateTable("./CreateTable.txt",C);
    // std::string execution = "set transaction isolation level repeatable read;";
    // ExecuteCommand(execution,C);
    // freopen("server_result.txt", "w", stdout); 

    //try link to the world locally

    int world_id;
    Warehouse w(1,2,3);
    for(;;){
        if((world_sock=Amazon_connect_to_world(23456,w,world_id))==-1){
            std::cout<<"Amazon: Failed to connect to world"<<std::endl;
            sleep(1);
            continue;
        }
        else{
            std::cout<<"Amazon: Connected to world"<<std::endl;
        }
        if((ups_sock=Amazon_wait_for_UPS(5688,world_id))==-1){
            std::cout<<"Amazon: Failed to connect to UPS"<<std::endl;
            sleep(1);
            continue;
        }
        else{
            std::cout<<"Amazon: Connected to UPS"<<std::endl;
        }

        // fd_set readfds;
        // while(true){
        //     FD_ZERO(&readfds);
        //     FD_SET(world_sock, &readfds);
        //     FD_SET(ups_sock, &readfds);
        //     int maxfd = std::max(world_sock, ups_sock);
        //     select(maxfd+1, &readfds, NULL, NULL, NULL);
        //     int bytes;
        //     std::vector<char> buf(10000);
        //     if(FD_ISSET(world_sock, &readfds)){
        //         bytes = recv(world_sock, buf.data(), buf.size(), 0);
        //     }
        //     if(FD_ISSET(ups_sock, &readfds)){
        //         // PRINTID std::cout<<"read from server"<<std::endl;
        //         std::vector<char> buf_server(10000);
        //         bytes = recv(ups_sock, buf_server.data(), buf_server.size(), 0);
        //     }
        // }
        while (true)
        {
            sleep(1);
        }
        

    }

    return 0;
}


