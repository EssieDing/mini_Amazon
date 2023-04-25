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
int Send_command_to_UPS()


#endif