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


// class WorldHandler {
// public:
//     WorldHandler() {}
//     void operator()() {
//         while(true){
//             receive_Aresponse_from_world();
//         }
//     }    
// };


// class UPSHandler {
// public:
//     UPSHandler() {}
//     void operator() (){
//         while(true){
//             receive_UACommands_from_UPS();
//         }
//     }
// };


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

        // pool.enqueue(WorldHandler());
        // pool.enqueue(UPSHandler());
        while (true)
        {
            sleep(1);
        }
        

    }

    return 0;
}


