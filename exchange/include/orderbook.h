#ifndef TRADING_ENGINE_ORDERBOOK_H
#define TRADING_ENGINE_ORDERBOOK_H

#include <map>
#include <unordered_map>
#include <queue>

class LimitOrder;
class MarketOrder;
class Limit;
class Exchange;

#include "order.h"
#include "limit.h"
#include "exchange.h"

class Orderbook {
    friend class Limit;
    friend class Exchange;
public:
    explicit Orderbook(std::string ticker);

    int num_orders() const { return _orderbook_orders; }

    float get_best_bid() const;

    float get_best_ask() const;

    void add_market_order(MarketOrder* new_order);

    void add_limit_order(LimitOrder* new_order);

    void cancel_order(LimitOrder* this_order);

    // void change_order(Order* this_order);

    float match_limit_orders(std::unordered_map<int, LimitOrder*> &orders_map);

    void match_market_orders(std::unordered_map<int, LimitOrder*> &orders_map);

private:
    // binary trees of bid and ask limits
    std::map<float, Limit, std::greater<>> _bids;
    std::map<float, Limit> _asks;

    // hash tables of bid and ask limits
    std::unordered_map<float, Limit*> _bids_table;
    std::unordered_map<float, Limit*> _asks_table;

    // queue of incoming market orders
    std::queue<MarketOrder*> _market_bids;
    std::queue<MarketOrder*> _market_asks;

    const std::string _ticker;

    int _bid_orders;
    int _bid_volume;

    int _ask_orders;
    int _ask_volume;

    int _orderbook_orders;
    int _orderbook_volume;

    Exchange *_parent_exchange;
};


#endif //TRADING_ENGINE_ORDERBOOK_H
