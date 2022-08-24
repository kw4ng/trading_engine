#include <iostream>

#include "../../../include/networking/server/exchange/exchange.h"

Exchange::Exchange() : _total_captured_spread(0), _total_orders(0), _total_volume(0) {}

int Exchange::add_order(orderType type, const std::string& ticker, float price, int volume, orderSide side) {
    // set random id and current time
    int id = rand() % 1000;
    std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();

    // create new orderbook for new ticker
    if (!_orderbooks.contains(ticker)) {
        auto* new_orderbook = new Orderbook(ticker);
        new_orderbook->_parent_exchange = this;

        _orderbooks.insert({ ticker, new_orderbook});
    }

    // create new market order
    if (type == orderType::market) {
        auto* new_order = new MarketOrder(id, ticker, volume, side, current_time);
        _orderbooks.at(ticker)->add_market_order(new_order);
    }
    // create new limit order
     else {
         auto* new_order = new LimitOrder(id, ticker, volume, price, side, current_time);
         _orderbooks.at(ticker)->add_limit_order(new_order);
         _orders[id] = new_order;
     }

     _total_orders++;
     _total_volume += volume;

     execute();

     return id;
}

void Exchange::cancel_order(int id) {
    // order exists
    if (_orders.contains(id)) {
        LimitOrder* this_order = _orders[id];

        _orderbooks.at(this_order->get_ticker())->cancel_order(this_order);

        _orders.erase(id);

        _total_orders--;
        _total_volume -= this_order->get_volume();
    }
    // order does not exist
    else {
        std::cerr << "order does not exist" << std::endl;
    }
}

void Exchange::execute() {
    for (auto orderbook : _orderbooks) {
        orderbook.second->match_market_orders(_orders);
        _total_captured_spread += orderbook.second->match_limit_orders(_orders);
    }
}

LimitOrder* Exchange::find_order(int id) {
    return _orders[id];
}
