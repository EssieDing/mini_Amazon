#ifndef COMMON_HPP
#define COMMON_HPP
#include<mutex>
#include<vector>
#include<thread>
#include<memory>
#include<cstdlib>
#include<bitset>
#include"ThreadPool.h"
#include"GPB_message.hpp"
#include"protobuf/world_amazon.pb.h"
#include"protobuf/amazon_ups.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

static long long server_seq_num=0;
std::mutex server_seq_num_mutex;
ThreadPool pool(50);

const char *world_ip="";
const char *ups_ip="";
const char *front_end_ip="127.0.0.1";
const int world_port=23456;
const int ups_port=5688;
const int front_end_port=12345;

static int world_sock=-1;
static int ups_sock=-1;

std::bitset<SIZE_MAX> send_acks; //whether receiver ack for sent message
std::bitset<SIZE_MAX> recv_acks; //whether finished received message

//For ApurchaseMore to retrive back shipid, update by send_ApurchaseMore_to_world
std::map<long long, long long> seqnum_to_shipid; 
//For Apack to retrive back truckid, update by Process_UAresponse (UATruckArrived)
std::map<long long, int> shipid_to_truckid; //


long long get_server_seq_num(){
    std::lock_guard<std::mutex> lock(server_seq_num_mutex);
    return server_seq_num++;
}




#endif
