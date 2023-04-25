#include "common.hpp"

long long server_seq_num = 0;
std::mutex server_seq_num_mutex;
ThreadPool pool(200);

const char *world_ip = "";
const char *ups_ip = "";
const char *front_end_ip = "127.0.0.1";
const int world_port = 23456;
const int ups_port = 5688;
const int front_end_port = 12345;

int world_sock = -1;
int ups_sock = -1;

std::bitset<10000> send_acks; //whether receiver ack for sent message
std::bitset<10000> recv_acks; //whether finished received message

std::map<long long, OrderInfo> seqnum_to_orderinfo;
std::map<long long, int> shipid_to_truckid;
std::map<long long, int> shipid_to_whid;