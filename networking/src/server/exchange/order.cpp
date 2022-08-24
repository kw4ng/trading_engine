#include <iostream>

#include "../../../include/networking/server/exchange/order.h"

LimitOrder::LimitOrder(int id, const std::string& ticker, int volume, float price, orderSide side,
                       const std::chrono::system_clock::time_point& time) : Order(id, ticker, volume, side, time),
                            _price(price), _partially_filled(false), _partial_volume(volume), _next(nullptr), _prev(nullptr), _parent_limit(nullptr) {}

void LimitOrder::partial_fill(int fill_volume) {
    _partially_filled = true;
    _partial_volume -= fill_volume;
}

void LimitOrder::print_order() {
    std::cout << "order " << get_id() << " executed at $" << get_price() << " per share: " << std::endl;

    std::cout << "\t id: " << get_id() << std::endl;
    std::cout << "\t ticker: " << get_ticker() << std::endl;
    std::string side = (get_side() == orderSide::buy) ? "buy" : "sell";
    std::cout << "\t side: " << side << std::endl;
    std::cout << "\t price: " << get_price() << std::endl;
    std::cout << "\t volume: " << get_volume() << std::endl;
    time_t tt = std::chrono::system_clock::to_time_t(get_time());
    std::cout << "\t time: " << ctime(&tt) << std::endl;
}

MarketOrder::MarketOrder(int id, const std::string &ticker, int volume, orderSide side,
                         const std::chrono::system_clock::time_point &time) : Order(id, ticker, volume, side, time) {}


