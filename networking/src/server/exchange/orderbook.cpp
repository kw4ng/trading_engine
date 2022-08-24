#include <algorithm>

#include "../../../include/networking/server/exchange/orderbook.h"

// initialize parent exchange
Orderbook::Orderbook(std::string ticker) : _ticker(std::move(ticker)), _bid_orders(0), _bid_volume(0), _ask_orders(0), _ask_volume(0), _orderbook_orders(0), _orderbook_volume(0), _parent_exchange(nullptr) {}


float Orderbook::get_best_bid() const {
    return _bids.begin()->second.get_price();
}

float Orderbook::get_best_ask() const {
    return _asks.begin()->second.get_price();
}

void Orderbook::add_market_order(MarketOrder *new_order) {
    // bid order
    if (new_order->get_side() == orderSide::buy) {
        _market_bids.push(new_order);
    }
    // ask order
     else {
         _market_asks.push(new_order);
     }

     _orderbook_orders++;
     _orderbook_volume += new_order->get_volume();
}

void Orderbook::add_limit_order(LimitOrder *new_order) {
    // bid order
    if (new_order->get_side() == orderSide::buy) {
        // limit does not exist yet
        if (!_bids_table.contains(new_order->get_price())) {
            _bids_table[new_order->get_price()] = &_bids[new_order->get_price()];
            _bids_table[new_order->get_price()]->_parent_orderbook = this;
        }

        _bids_table[new_order->get_price()]->add_order(new_order);

        _bid_orders++;
        _bid_volume += new_order->get_volume();
    }
    // ask order
    else if (new_order->get_side() == orderSide::sell) {
        // limit does not exist yet
        if (!_asks_table.contains(new_order->get_price())) {
            _asks_table[new_order->get_price()] = &_asks[new_order->get_price()];
            _asks_table[new_order->get_price()]->_parent_orderbook = this;
        }

        _asks_table[new_order->get_price()]->add_order(new_order);

        _ask_orders++;
        _ask_volume += new_order->get_volume();
    }

    _orderbook_orders++;
    _orderbook_volume += new_order->get_volume();
}

void Orderbook::cancel_order(LimitOrder *this_order) {
    // bid order
    if (this_order->get_side() == orderSide::buy) {
        _bids_table[this_order->get_price()]->cancel_order(this_order);

        if (_bids_table[this_order->get_price()]->is_empty()) {
            _bids.erase(this_order->get_price());
            _bids_table.erase(this_order->get_price());
        }

        _bid_orders--;
        _bid_volume -= this_order->get_volume();
    }
    // ask order
    else if (this_order->get_side() == orderSide::sell) {
        _asks_table[this_order->get_price()]->cancel_order(this_order);

        if (_asks_table[this_order->get_price()]->is_empty()) {
            _asks.erase(this_order->get_price());
            _asks_table.erase(this_order->get_price());
        }

        _ask_orders--;
        _ask_volume -= this_order->get_volume();
    }

    _orderbook_orders--;
    _orderbook_volume -= this_order->get_volume();
}

float Orderbook::match_limit_orders(std::unordered_map<int, LimitOrder *> &orders_map) {
    // no bids or asks
    if (_bids.empty() || _asks.empty()) {
        return 0;
    }

    auto bids_it = _bids.begin();
    auto asks_it = _asks.begin();

    float spread = 0;

    // fulfill limit orders when lowest ask is less than highest bid
    while (get_best_ask() <= get_best_bid()) {
        int volume_to_fulfill = std::min(bids_it->second.get_volume(), asks_it->second.get_volume());

        // need to reset best bid
        if (bids_it->second.limit_fulfill_order(orders_map, volume_to_fulfill, spread)) {
            _bids_table.erase(bids_it->second.get_price());

            _bids.erase(bids_it);
            bids_it = _bids.begin();
        }
        // need to reset best ask
        if (asks_it->second.limit_fulfill_order(orders_map, volume_to_fulfill, spread)) {
            _asks_table.erase(asks_it->second.get_price());

            _asks.erase(asks_it);
            asks_it = _asks.begin();
        }
    }

    return spread;
}

void Orderbook::match_market_orders(std::unordered_map<int, LimitOrder *> &orders_map) {
    // edge cases
    if (_market_bids.empty() && _market_asks.empty()) {
        return;
    } else if (_market_bids.empty() && (!_market_asks.empty() && _market_asks.front()->get_volume() > _bid_volume)) {
        return;
    } else if (_market_asks.empty() && (!_market_bids.empty() && _market_bids.front()->get_volume() > _ask_volume)) {
        return;
    } else if ((!_market_bids.empty() && _market_bids.front()->get_volume() > _ask_volume) && (!_market_asks.empty() && _market_asks.front()->get_volume() > _bid_volume)) {
        return;
    }

    auto bids_it = _bids.begin();
    auto asks_it = _asks.begin();

    bool no_valid_bid_orders = false;
    bool no_valid_ask_orders = false;

    while (!_market_bids.empty() || !_market_asks.empty()) {
        if (_market_bids.empty() || _market_bids.front()->get_volume() > _ask_volume) no_valid_bid_orders = true;
        if (_market_asks.empty() || _market_asks.front()->get_volume() > _bid_volume) no_valid_ask_orders = true;

        MarketOrder* this_order;

        // no valid market orders
        if (no_valid_bid_orders && no_valid_ask_orders) break;
        // only ask orders valid
        else if (no_valid_bid_orders) this_order =_market_asks.front();
        // only bid orders valid
        else if (no_valid_ask_orders) this_order = _market_bids.front();
        // get first market order in queue
        else this_order = (_market_bids.front()->get_time() < _market_asks.front()->get_time()) ? _market_bids.front() : _market_asks.front();

        int volume = this_order->get_volume();
        float avg_price = 0;

        // buy market order
        if (this_order->get_side() == orderSide::buy) {
            while (volume > 0) {
                if (asks_it->second.market_fulfill_order(orders_map, volume, avg_price)) {
                    _asks_table.erase(asks_it->second.get_price());

                    _asks.erase(asks_it);
                    asks_it = _asks.begin();
                }
            }
        }
        // sell market order
        else if (this_order->get_side() == orderSide::sell) {
            while (volume > 0) {
                if (bids_it->second.market_fulfill_order(orders_map, volume, avg_price)) {
                    _bids_table.erase(bids_it->second.get_price());

                    _bids.erase(bids_it);
                    bids_it = _bids.begin();
                }
            }
        }

        // update average market price
        avg_price /= (float)this_order->get_volume();

        // print market order information

        // pop from appropriate queue
        this_order->get_side() == orderSide::buy ? _market_bids.pop() : _market_asks.pop();

        // update order/volume data
        _orderbook_orders--;
        _orderbook_volume -= this_order->get_volume();

        // exchange
    }
}
