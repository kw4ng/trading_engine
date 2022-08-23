#ifndef TRADING_ENGINE_ORDER_H
#define TRADING_ENGINE_ORDER_H

#include <string>
#include <chrono>

class Limit;

enum class orderSide {
    buy,
    sell
};

enum class orderType {
    market,
    limit
};

class Order {
public:
    int get_id() const { return _id; }

    std::string get_ticker() const { return _ticker; }

    int get_volume() const { return _volume; }

    orderSide get_side() const { return _side; }

    std::chrono::system_clock::time_point get_time() const { return _time; }

    Order(int id, std::string ticker, int volume, orderSide side, std::chrono::system_clock::time_point time) : _id(id), _ticker(ticker), _volume(volume), _side(side), _time(time) {}

protected:
    int _id;
    std::string _ticker;
    int _volume;
    orderSide _side;
    std::chrono::system_clock::time_point _time;
};

class LimitOrder : public Order {
public:
    LimitOrder(int id, const std::string &ticker, int volume, float price, orderSide side, const std::chrono::system_clock::time_point &time);

    float get_price() const { return _price; }

    int get_partial_volume() const { return _partial_volume; }

    void partial_fill(int fill_volume);

    void print_order();

public:
    LimitOrder* _next;
    LimitOrder* _prev;

    Limit* _parent_limit;

private:
    float _price;

    int _partial_volume;
    bool _partially_filled;
};

class MarketOrder : public Order {
public:
    MarketOrder(int id, const std::string &ticker, int volume, orderSide side, const std::chrono::system_clock::time_point &time);
};


#endif //TRADING_ENGINE_ORDER_H
