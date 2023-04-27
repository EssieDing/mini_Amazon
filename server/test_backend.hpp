#ifndef TEST_BACKEND_HPP
#define TEST_BACKEND_HPP
#include "common.hpp"
#include "world_handle.hpp"

int test_send_Apurchasemore(){
    std::vector<AProduct> products;
    AProduct aproduct;
    aproduct.set_id(1);
    aproduct.set_description("test");
    aproduct.set_count(1);
    products.push_back(aproduct);
    send_ApurchaseMore_to_world(1,products,1,"test",1,1);
    return 0;
}

#endif