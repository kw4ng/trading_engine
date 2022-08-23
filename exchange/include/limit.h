#ifndef TRADING_ENGINE_LIMIT_H
#define TRADING_ENGINE_LIMIT_H

#include <unordered_map>

class LimitOrder;
class MarketOrder;
class Orderbook;

#include "order.h"
#include "orderbook.h"

class Limit {
    friend class Order;
    friend class Orderbook;
    friend class Exchange;
public:
    Limit();

    bool is_empty() { return _head == nullptr && _tail == nullptr; }

    float get_price() const { return _price; }

    int get_volume() const { return _limit_volume; }

    LimitOrder* get_head_order() const { return _head; }

    void add_order(LimitOrder* new_order);

    void cancel_order(LimitOrder* this_order);

    // void change_order(LimitOrder* this_order);

    bool limit_fulfill_order(std::unordered_map<int, LimitOrder*> &orders_map, int volume, float &spread);

    bool market_fulfill_order(std::unordered_map<int, LimitOrder*> &orders_map, int &volume, float &avg_price);

private:
    float _price;
    int _limit_orders;
    int _limit_volume;

    LimitOrder* _head;
    LimitOrder* _tail;

    Orderbook* _parent_orderbook{};
};


#endif //TRADING_ENGINE_LIMIT_H
