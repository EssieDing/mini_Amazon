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
    while(true){
        // whether continue looping
        if(seq_num!=-1 && send_acks[seq_num]==true){
            break;
        }
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
        // seq_num==-1 means send ack back, no need to wait for ack
        if(seq_num==-1) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }       
    

    return 0;
}

int send_ApurchaseMore_to_world(int wh_id,std::vector<AProduct> &products,\
        long long packageid,std::string accountname,int deliver_x,int deliver_y){
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
    OrderInfo orderinfo{packageid,accountname,deliver_x,deliver_y};
    seqnum_to_orderinfo[seq_num]=orderinfo;
    shipid_to_whid[packageid]=wh_id;
    pool.enqueue(Send_command_to_world,acommands,seq_num);
    return 0;
}

int send_APack_to_world(int wh_id,google::protobuf::RepeatedPtrField<AProduct> products,long long shipid){
    APack apack;
    apack.set_whnum(wh_id);
    for(auto &p:products){
        auto thing = apack.add_things();
        *thing=std::move(p);
    }
    apack.set_shipid(shipid);
    long long seq_num=get_server_seq_num();
    apack.set_seqnum(seq_num);
    send_acks[seq_num]=false;
    ACommands acommands;
    acommands.add_load()->CopyFrom(apack);
    pool.enqueue(Send_command_to_world,acommands,seq_num);
    return 0;
}

int send_APutOnTruck_to_world(int wh_id,int truckid, int shipid){
    APutOnTruck aputontruck;
    aputontruck.set_whnum(wh_id);
    aputontruck.set_truckid(truckid);
    aputontruck.set_shipid(shipid);
    long long seq_num=get_server_seq_num();
    aputontruck.set_seqnum(seq_num);
    send_acks[seq_num]=false;
    ACommands acommands;
    acommands.add_load()->CopyFrom(aputontruck);
    pool.enqueue(Send_command_to_world,acommands,seq_num);
    return 0;
}

// int send_AQuery_to_world(int wh_id,std::vector<AQuery> &queries){

// }

int send_acks_to_world(int ack){
    ACommands acommands;
    acommands.add_acks(ack);
    pool.enqueue(Send_command_to_world,acommands,-1);
    return 0;
}







//--------------------receive message from world--------------------

//parsing Aresponse
//TO-DO: finished, packagestatus

int Process_Arrived(APurchaseMore now_arrived){
    if(recv_acks[now_arrived.seqnum()]==true){
        send_acks_to_world(now_arrived.seqnum());
    }
    recv_acks[now_arrived.seqnum()]=true;
    auto it = seqnum_to_orderinfo.find(now_arrived.seqnum());
    if(it==seqnum_to_orderinfo.end()){
        std::cout<<"Amazon: seqnum_to_orderinfo not found"<<std::endl;
        return -1;
    }
    OrderInfo &now_orderinfo=it->second;
    send_APack_to_world(now_arrived.whnum(),now_arrived.things(),now_orderinfo.package_id);
    //TO-DO： update order status to be packing
    Update_Order_Status(now_orderinfo.package_id,"packing");

    AUDeliveryLocation audeliverylocation;
    audeliverylocation.set_x(now_orderinfo.delivery_x);
    audeliverylocation.set_y(now_orderinfo.delivery_y);
    Send_AUInitPickUP_to_UPS(now_orderinfo.package_id,now_orderinfo.account_name,\
        &audeliverylocation,now_arrived.things());

    
    send_acks_to_world(now_arrived.seqnum());
    return 0;
}

int Process_APacked(APacked now_packed){
    if(recv_acks[now_packed.seqnum()]==true){
        send_acks_to_world(now_packed.seqnum());
    }
    recv_acks[now_packed.seqnum()]=true;
    Update_Order_Status(now_packed.shipid(),"packed");
    while(shipid_to_truckid.find(now_packed.shipid())==shipid_to_truckid.end()){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    Update_Order_Status(now_packed.shipid(),"loading");
    send_APutOnTruck_to_world(shipid_to_whid[now_packed.shipid()],shipid_to_truckid[now_packed.shipid()],now_packed.shipid());
   
    send_acks_to_world(now_packed.seqnum());
    return 0;
}

int Process_Aresponse(AResponses &aresponses){
    //Parchase more arrived received
    //Amazon: No.8 send Apack to world
    //DB: update order status to be packing
    //Amazon: No.8 send AUInitPickUp to UPS
    for(auto &now_arrived:aresponses.arrived()){
        pool.enqueue(Process_Arrived,now_arrived);
    }
    //Apacked ready received
    //check whether UATruckArrived received
    //if yes, send APutOnTruck to world
    //else wait for UATruckArrived
    //DB: update order status to be packed
    for(auto &now_packed:aresponses.ready()){
        pool.enqueue(Process_APacked,now_packed);

    }

    //ALoaded loaded received
    //Send AULoaded to UPS
    for(auto &now_loaded:aresponses.loaded()){
        if(recv_acks[now_loaded.seqnum()]==true){
            send_acks_to_world(now_loaded.seqnum());
        }
        recv_acks[now_loaded.seqnum()]=true;
        Update_Order_Status(now_loaded.shipid(),"delivering");
        Send_AULoaded_to_UPS(now_loaded.shipid());
        send_acks_to_world(now_loaded.seqnum());
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
    return 0;
}

int receive_Aresponse_from_world(){
    AResponses aresponses;
    try{ 
        std::unique_ptr<GPBFileInputStream> input(new GPBFileInputStream(world_sock));
        if(recvMesgFrom(aresponses,input.get())!=true){
            throw std::runtime_error("receive Aresponse from world failed");
        }
    }catch(std::exception &e){
        std::cout<<"Amazon: receive Aresponse from world failed"<<std::endl;
        return -1;
    }
    if(Process_Aresponse(aresponses)!=0){
        std::cout<<"Amazon: process Aresponse failed"<<std::endl;
        return -1;
    }
    return 0;
}


#endif