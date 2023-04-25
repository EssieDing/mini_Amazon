#ifndef WORLD_HANDLE_HPP
#define WORLD_HANDLE_HPP
#include<mutex>
#include<vector>
#include<thread>
#include<memory>
#include<cstdlib>
#include<exception>
#include<bitset>
#include"ThreadPool.h"
#include"GPB_message.hpp"
#include"protobuf/world_amazon.pb.h"
#include"protobuf/amazon_ups.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "common.hpp"
#include "UPS_handle.hpp"



//--------------------send message to world--------------------


int Send_command_to_world(ACommands &acommands,int seq_num=-1){
    while(send_acks[seq_num]!=true){
        try{
            //send to world
            std::unique_ptr<GPBFileOutputStream> output(new GPBFileOutputStream(world_sock));
            if(sendMesgTo(acommands,output.get())!=true){
                throw std::runtime_error("send command to world failed");
            }
        }catch(std::exception &e){
            std::cout<<"Amazon: send command to world failed"<<std::endl;
            return -1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }       
    

    return 0;
}

int send_ApurchaseMore_to_world(int wh_id,std::vector<AProduct> &products,long long shipid){
    APurchaseMore apurchasemore;
    apurchasemore.set_whnum(wh_id);
    for(auto &p:products){
        auto apurchase = apurchasemore.add_things();
        *apurchase=std::move(p);
    }
    long long seq_num=get_server_seq_num();
    apurchasemore.set_seqnum(seq_num);
    send_acks[seq_num]=false;
    ACommands acommands;
    acommands.add_buy()->CopyFrom(apurchasemore);
    seqnum_to_shipid[seq_num]=shipid;
    pool.enqueue(Send_command_to_world,acommands,seq_num);
    return 0;
}

int send_APack_to_world(int wh_id,auto &packs,long long shipid){

}

int send_APutOnTruck_to_world(int wh_id,std::vector<APutOnTruck> &put_on_trucks){

}

int send_AQuery_to_world(int wh_id,std::vector<AQuery> &queries){

}

int send_acks_to_world(int &ack){

}







//--------------------receive message from world--------------------
int receive_Aresponse_from_world(AResponses &aresponses){
    try{
        std::unique_ptr<GPBFileInputStream> input(new GPBFileInputStream(world_sock));
        if(recvMesgFrom(aresponses,input.get())!=true){
            throw std::runtime_error("receive Aresponse from world failed");
        }
    }catch(std::exception &e){
        std::cout<<"Amazon: receive Aresponse from world failed"<<std::endl;
        return -1;
    }
    return 0;
}

//parsing Aresponse
//TO-DO: finished, packagestatus

int Process_Aresponse(AResponses &aresponses){
    //Parchase more arrived received
    //Amazon: No.8 send Apack to world
    //Amazon: No.8 send AUInitPickUp to UPS
    for(auto &now_arrived:aresponses.arrived()){
        send_APack_to_world(now_arrived.whnum(),now_arrived.things(),seqnum_to_shipid[now_arrived.seqnum()]);
        //TO-DO
        //send_AUInitPickUp_to_UPS(now_arrived.whnum(),now_arrived.things(),seqnum_to_shipid[now_arrived.seqnum()]);
    }
    //Apacked ready received
    //check whether UATruckArrived received
    //if yes, send APutOnTruck to world
    //else wait for UATruckArrived
    for(auto &now_packed:aresponses.ready()){
        if(now_packed.has_truckid()){
            //TO-DO
            //send_APutOnTruck_to_world(now_packed.whnum(),now_packed.things(),now_packed.truckid());
        }else{
            //TO-DO
            //send_AQuery_to_world(now_packed.whnum(),now_packed.things());
        }
    }

    //ALoaded loaded received
    //Send AULoaded to UPS
    for(auto &now_loaded:aresponses.loaded()){
        //TO-DO
        //send_AULoaded_to_UPS(now_loaded.truckid());
    }

    //AErr error received
    //Print error message 
    for(auto &now_error:aresponses.error()){
        std::cout<<"Amazon: ereor: "<<now_error.err()<<" originseqnum: "<<now_error.originseqnum()<<std::endl;
        send_acks_to_world(now_error.seqnum());
    }

    //acks received from world
    //update bitset
    for(auto &now_ack:aresponses.acks()){
        send_acks[now_ack]=true;
    }
}

#endif