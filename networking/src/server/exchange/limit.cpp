#include "../../../include/networking/server/exchange/limit.h"

// need to initialize parent orderbook
Limit::Limit() : _price(0), _limit_orders(0), _limit_volume(0), _head(nullptr), _tail(nullptr), _parent_orderbook(nullptr) {}

void Limit::add_order(LimitOrder *new_order) {
    // new order at empty price level
    if (is_empty()) {
        _head = new_order;
        _tail = new_order;
        _price =  new_order->get_price();
    }
    // append to tail of list
    else {
        LimitOrder* tail_order = _tail;
        new_order->_prev = tail_order;
        tail_order->_next = new_order;
        _tail = new_order;
    }

    new_order->_parent_limit = this;

    _limit_orders++;
    _limit_volume += new_order->get_volume();
}

void Limit::cancel_order(LimitOrder *this_order) {
    // order in middle of list
    if (this_order->_prev != nullptr && this_order->_next != nullptr) {
        this_order->_prev->_next = this_order->_next;
        this_order->_next->_prev = this_order->_prev;
    }
    // order at beginning of list
    else if (this_order->_prev == nullptr && this_order->_next != nullptr) {
        this_order->_next->_prev = nullptr;
    }
    // order at end of list
    else if (this_order->_prev != nullptr && this_order->_next == nullptr) {
        this_order->_prev->_next = nullptr;
    }

    // reset head and tail
    if (_head == this_order && _tail == this_order) {
        _head = nullptr;
        _tail = nullptr;
    }
    // order at head
    else if (_head == this_order) {
        _head = this_order->_next;
    }
    // order at tail
    else if (_tail == this_order) {
        _tail = this_order->_prev;
    }

    _limit_orders--;
    _limit_volume -= this_order->get_volume();

    delete this_order;
}

bool Limit::limit_fulfill_order(std::unordered_map<int, LimitOrder *> &orders_map, int volume, float &spread) {
    orderSide head_side = _head->get_side();

    while (volume > 0) {
        LimitOrder* this_order = _head;

        // head order is fulfilled
        if (this_order->get_partial_volume() <= volume) {
            // print order information
            this_order->print_order();

            // update spread
            if (head_side == orderSide::buy) {
                spread += (float)this_order->get_partial_volume() * this_order->get_price();
            } else {
                spread -= (float)this_order->get_partial_volume() * this_order->get_price();
            }

            // update order/volume data
            volume -= this_order->get_partial_volume();

            _limit_orders--;
            _limit_volume -= this_order->get_partial_volume();

            if (head_side == orderSide::buy) {
                this_order->_parent_limit->_parent_orderbook->_bid_orders--;
                this_order->_parent_limit->_parent_orderbook->_bid_volume -= this_order->get_partial_volume();
            } else {
                this_order->_parent_limit->_parent_orderbook->_ask_orders--;
                this_order->_parent_limit->_parent_orderbook->_ask_volume -= this_order->get_partial_volume();
            }

            this_order->_parent_limit->_parent_orderbook->_orderbook_orders--;
            this_order->_parent_limit->_parent_orderbook->_orderbook_volume -= this_order->get_partial_volume();

            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_orders--;
            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_volume -= this_order->get_partial_volume();

            // erase order
            orders_map.erase(this_order->get_id());

            // check if order was only one at list, else need to update pointers
            if (_head == this_order && _tail == this_order) {
                _head = nullptr;
                _tail = nullptr;
            } else {
                _head = this_order->_next;
                this_order->_next->_prev = nullptr;
            }

            delete this_order;
        }
        // head order is partially filled
        else {
            this_order->partial_fill(volume);

            // update order/volume data
            _limit_volume -= volume;

            if (head_side == orderSide::buy) {
                this_order->_parent_limit->_parent_orderbook->_bid_volume -= volume;
            } else {
                this_order->_parent_limit->_parent_orderbook->_ask_volume -= volume;
            }

            this_order->_parent_limit->_parent_orderbook->_orderbook_volume -= volume;

            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_volume -= volume;

            // update spread
            if (head_side == orderSide::buy) {
                spread += (float)(this_order->get_volume() - this_order->get_partial_volume()) * this_order->get_price();
            } else {
                spread -= (float)(this_order->get_volume() - this_order->get_partial_volume()) * this_order->get_price();
            }

            volume = 0;
        }
    }

    return _limit_volume == 0;
}

bool Limit::market_fulfill_order(std::unordered_map<int, LimitOrder *> &orders_map, int &volume, float &avg_price) {
    orderSide head_side = _head->get_side();

    while (volume > 0 && _limit_volume > 0) {
        LimitOrder* this_order = _head;

        // head order is fulfilled
        if (this_order->get_partial_volume() <= volume) {
            // print order information
            this_order->print_order();

            // update average market price
            avg_price += (float)this_order->get_partial_volume() * this_order->get_price();

            // update order/volume data
            volume -= this_order->get_partial_volume();

            _limit_orders--;
            _limit_volume -= this_order->get_partial_volume();

            if (head_side == orderSide::buy) {
                this_order->_parent_limit->_parent_orderbook->_bid_orders--;
                this_order->_parent_limit->_parent_orderbook->_bid_volume -= this_order->get_partial_volume();
            } else {
                this_order->_parent_limit->_parent_orderbook->_ask_orders--;
                this_order->_parent_limit->_parent_orderbook->_ask_volume -= this_order->get_partial_volume();
            }

            this_order->_parent_limit->_parent_orderbook->_orderbook_orders--;
            this_order->_parent_limit->_parent_orderbook->_orderbook_volume -= this_order->get_partial_volume();

            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_orders--;
            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_volume -= this_order->get_partial_volume();

            // erase order
            orders_map.erase(this_order->get_id());

            // check if order was only one at limit, else update pointers
            if (_head == this_order && _tail == this_order) {
                _head = nullptr;
                _tail = nullptr;
            } else {
                _head = this_order->_next;
                this_order->_next->_prev = nullptr;
            }

            delete this_order;
        }
        // head order is partially filled
        else {
            this_order->partial_fill(volume);

            // update order/volume data
            _limit_volume -= volume;

            if (head_side == orderSide::buy) {
                this_order->_parent_limit->_parent_orderbook->_bid_volume -= volume;
            } else {
                this_order->_parent_limit->_parent_orderbook->_ask_volume -= volume;
            }

            this_order->_parent_limit->_parent_orderbook->_orderbook_volume -= volume;

            this_order->_parent_limit->_parent_orderbook->_parent_exchange->_total_volume -= volume;

            // update average market price
            avg_price += (float)volume * this_order->get_price();

            volume = 0;
        }
    }

    return _limit_volume == 0;
}
