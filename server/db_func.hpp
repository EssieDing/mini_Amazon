#ifndef __DB_FUNC_H__
#define __DB_FUNC_H__

#include<iostream>
#include<string>
#include<pqxx/pqxx>
#include<fstream>
#include<chrono>
#include<ctime>
#include<assert.h>
#include<mutex>

typedef long long ll;

long curtime(){
    time_t now = time(0);
    long curtime = static_cast<long>(now);
    return curtime;
}

void ExecuteCommand(std::string command, pqxx::connection *C){
    pqxx::work W(*C);
    W.exec(command);
    W.commit();
}

void DropTable(pqxx::connection *C){
    std::vector<std::string> tablename{"SYMBOL","ACCOUNT","POSITION","TRANSACTION","EXECUTION","CANCELATION"};
    for(auto name:tablename){
        std::string query = "DROP TABLE IF EXISTS "+name+" CASCADE;";
        ExecuteCommand(query,C);
    }
}

void CreateTable(std::string filename, pqxx::connection *C){
    std::ifstream f(filename);
    std::string command,line;
    if(f.is_open()){
        while(std::getline(f,line)){
            command+=line;
        }
        //std::cout<<command<<std::endl;
        ExecuteCommand(command,C);
        std::cout<<"Successfully Created tables"<<std::endl;
        f.close();
    }else{
        std::cerr<<"Cannot open file for creating tables"<<std::endl;
        exit(EXIT_FAILURE);
    }
}


#endif