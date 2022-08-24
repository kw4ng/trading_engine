#ifndef TRADING_ENGINE_EXCHANGE_H
#define TRADING_ENGINE_EXCHANGE_H

#include <unordered_map>

class Orderbook;

#include "orderbook.h"

class Exchange {
    friend class Limit;
public:
    Exchange();

    int add_order(orderType type, const std::string& ticker, float price, int volume, orderSide side);

    void cancel_order(int id);

    // void change_order();

    void execute();

    LimitOrder* find_order(int id);

private:
    // hash table of all orderbooks
    std::unordered_map<std::string, Orderbook*> _orderbooks;

    // hash table of all orders
    std::unordered_map<int, LimitOrder*> _orders;

    float _total_captured_spread;
    int _total_orders;
    int _total_volume;
};


#endif //TRADING_ENGINE_EXCHANGE_H
