#ifndef UPS_HANDLE_HPP
#define UPS_HANDLE_HPP
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
#include "world_handle.hpp"

//--------------------send message to ups--------------------
int Send_command_to_UPS(AUCommands aucommands,int seq_num=-1){
    
    while(true){
        // whether continue looping
        if(send_acks[seq_num]==true){
            if(seq_num!=-1){
                break;
            }
        }
        try{
            //send to ups
            std::unique_ptr<GPBFileOutputStream> output(new GPBFileOutputStream(ups_sock));
            if(sendMesgTo(aucommands,output.get())!=true){
                throw std::runtime_error("send command to ups failed");
            }
        }catch(std::exception &e){
            std::cout<<"Amazon: send command to ups failed"<<std::endl;
            return -1;
        }
        // seq_num==-1 means send ack back, no need to wait for ack
        if(seq_num==-1) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }       
    
    return 0;
}

int Send_AUInitPickUP_to_UPS(int wh_id,std::string accountname,AUDeliveryLocation* location, google::protobuf::RepeatedPtrField<AProduct> product){

    AUInitPickUp auinitpickup;
    auinitpickup.set_whid(wh_id);
    auinitpickup.set_accountname(accountname);
    auinitpickup.set_allocated_deliverylocation(location);
    for(auto &p:product){
        auto aproduct = auinitpickup.add_product();
        aproduct->set_id(p.id());
        aproduct->set_description(p.description());
        aproduct->set_count(p.count());
    }
    long long seq_num=get_server_seq_num();
    auinitpickup.set_seqnum(seq_num);
    send_acks[seq_num]=false;
    AUCommands aucommands;
    aucommands.add_pickupreq()->CopyFrom(auinitpickup);
    std::cout<<"Send_AUInitPickUP_to_UPS seq_num: "<<seq_num<<std::endl;
    // std::thread sending_thread(Send_command_to_UPS,aucommands,seq_num);
    // sending_thread.detach();
    return 0;
}

int Send_AULoaded_to_UPS(int shipid){
    AULoaded auloaded;
    auloaded.set_shipid(shipid);
    long long seq_num=get_server_seq_num();
    auloaded.set_seqnum(seq_num);
    send_acks[seq_num]=false;
    AUCommands aucommands;
    aucommands.add_loaded()->CopyFrom(auloaded);
    std::cout<<"Send_AULoaded_to_UPS seq_num: "<<seq_num<<std::endl;
    // std::thread sending_thread (Send_command_to_UPS,aucommands,seq_num);
    // sending_thread.detach();
    return 0;
}


int Send_ack_to_UPS(int ack){
    AUCommands aucommands;
    aucommands.add_acks(ack);
    std::cout<<"Send_received ack to UPS ack: "<<ack<<std::endl;
    // std::thread  sending_thread(Send_command_to_UPS,aucommands);
    // sending_thread.detach();
    return 0;
}

//--------------------receive message from ups--------------------
int Process_UACommands(UACommands uacommands){
    // TruckArrived received
    // update truck status
    // send ack to ups
    for(auto &now_truckarrived:uacommands.truckarrived()){
        //receive the commands before
        if(recv_acks[now_truckarrived.seqnum()]==true){
                   Send_ack_to_UPS(now_truckarrived.seqnum());
        }
        shipid_to_truckid[now_truckarrived.shipid()]=now_truckarrived.truckid();
        
        Send_ack_to_UPS(now_truckarrived.seqnum());
        recv_acks[now_truckarrived.seqnum()]=true;
    }

    // UADelievered received
    // DB: update order status
    // send ack to ups
    for(auto &now_delivered:uacommands.delivered()){
        if(recv_acks[now_delivered.seqnum()]==true){
            Send_ack_to_UPS(now_delivered.seqnum());

        }
        Update_Order_Status(now_delivered.packageid(),"delivered");
        Send_ack_to_UPS(now_delivered.seqnum());
        recv_acks[now_delivered.seqnum()]=true;
    }

    //acks received
    //update acks
    for(auto &now_ack:uacommands.acks()){
        send_acks[now_ack]=true;
    }
    return 0;
}


int receive_UACommands_from_UPS(){
    std::cout<<"UPS receiver running"<<std::endl;
    UACommands uacommands;
    try{
        //receive from ups
        std::unique_ptr<GPBFileInputStream> input(new GPBFileInputStream(ups_sock));
        if(recvMesgFrom(uacommands,input.get())!=true){
            throw std::runtime_error("receive command from ups failed");
        }
    }catch(std::exception &e){
        std::cout<<"Amazon: receive command from ups failed"<<std::endl;
        return -1;
    }
    std::cout<<"receive_UACommands_from_UPS"<<std::endl;
    Process_UACommands(std::move(uacommands));
    return 0;
}


void UPSHandle(){
    while(true){
        receive_UACommands_from_UPS();
    }
}




#endif