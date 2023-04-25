#include <iostream>

using namespace std;

class Order {
    public:
    string userName;
    int order_id;
    int addr_x;
    int addr_y;
    int ups_id;
    int item_id[20];
    int item_quantity[20];
    string item_description[20];

    public:
    Order(){}
    Order(const string & str){

    }

    
}