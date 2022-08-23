#include <iostream>

#include "exchange.h"

using namespace std;

int main() {
    cout << "hi" << endl;

//    Exchange e;
//
//    e.add_order(orderType::market, "TSLA", 0, 100, orderSide::buy);
//    e.add_order(orderType::market, "APPL", 0, 100, orderSide::sell);
//
//    e.add_order(orderType::limit, "TSLA", 100, 500, orderSide::buy);
//    e.add_order(orderType::limit, "APPL", 0, 100, orderSide::sell);

    Exchange exchange;


    // i want to add a limit order for apple at $100, 100 shares, buy


    // exchange.add_order(limit, "APPL", 100, 100, buy);



    exchange.add_order(orderType::market, "APPL", 0, 40, orderSide::buy);
    exchange.add_order(orderType::market, "APPL", 0, 120, orderSide::sell);

    int order1, order2, order3, order4, order5, order6, order7;

    order1 = exchange.add_order(orderType::limit, "APPL", 100, 100, orderSide::buy);
    order2 = exchange.add_order(orderType::limit, "APPL", 99, 150, orderSide::buy);
    order3 = exchange.add_order(orderType::limit, "APPL", 98, 130, orderSide::buy);

    order4 = exchange.add_order(orderType::limit, "APPL", 101, 100, orderSide::sell);
    order7 = exchange.add_order(orderType::limit, "APPL", 101, 50, orderSide::sell);
    order5 = exchange.add_order(orderType::limit, "APPL", 102, 150, orderSide::sell);
    order6 = exchange.add_order(orderType::limit, "APPL", 103, 130, orderSide::sell);

//    exchange.print_order(order1);
//    exchange.print_order(order2);
//    exchange.print_order(order3);
//    exchange.print_order(order4);
//    exchange.print_order(order5);
//    exchange.print_order(order6);

    cout << "adding matchable orders" << endl;


    // when head ask order volume is greater
    // exchange.add_order(limit, "APPL", 101, 50, buy);

    // when head ask order volume is less
    exchange.add_order(orderType::limit, "APPL", 102, 120, orderSide::buy);

    // should fulfill all of order 4, then leave with 20 more volume on buy side

    cout << "finished" << endl;
}
