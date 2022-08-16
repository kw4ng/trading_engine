#include <iostream>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <utility>
#include <vector>
#include <queue>
#include <list>
#include <thread>
#include <mutex>

using namespace std;

/*
 
    ORDERBOOK FEATURES:
        - process limit and market orders
        - add, cancel, and change order functions
        - FIFO w/ partially fulfilled orders (maybe add PRORATA)
        - acting as market maker to capture spread
 
 */

/*

 TODO:
 
 - change order function
 - server/client application
 - multithreading

*/


/*
 
 HOW ORDERS ARE FULFILLED
 
 
 - IF SELL ORDER SITS ON BOOK AT 99, AND BUY ORDER COMES IN FOR 101
    - ORDER FULFILLED AT 99, SELLER GETS 99 AND BUY GETS 99, MARKET MAKER GETS 0
 
 - IF BUY ORDER SITS ON BOOK AT 101, AND SELL ORDER COMES IN FOR 99
    - ORDER FULFILLED AT 101, BUYER BUYERS FOR 101 AND SELLER SELLS FOR 101
 
 
 - IF SELL ORDER SITS ON BOOK AT 99 AND 98, AND BUY ORDER COMES IN FOR 101
    - ORDER FULFILLED AT 98, SELLER GETS 99 AND BUYER GETS 98, MARKET MAKER GETS 0
 
 
 - ASKS     102
            101
            100
 
 - BIDS     99
            98
            97
 
 - IF THERE WAS MARKET MAKER IN THE MIDDLE, THEN:
    - MM BUYS SELL ORDER AT 99, SELLER GETS 99, THEN SELLS TO BUYER FOR 101, BUYER GETS 101, MM GETS 2
    - MM BUYS SELL ORDER AT 98, SELLER GETS 98, THEN SELLS TO BUYER FOR 101, BUYER GETS 101, MM GETS 3
 
 */

enum orderSide : bool {
    buy,
    sell
};

enum orderType : bool {
    market,
    limit
};

enum changeOrderType : char {
    price,
    volume,
    price_and_volume
};


class Exchange {
    
public:
    class Order;
    class Limit;
    class Orderbook;

public:
    
    class Order {
    protected:
        int id;
        string ticker;
        int volume;
        orderSide side;
        std::chrono::system_clock::time_point time;
        
    public:
        
        int get_id() const {
            return id;
        }
        
        string get_ticker() const {
            return ticker;
        }
        
        int get_volume() const {
            return volume;
        }
        
        orderSide get_side() {
            return side;
        }
        
        std::chrono::system_clock::time_point get_time() const {
            return time;
        }

    };
    
    class LimitOrder : public Order {
    private:
        float price;
        
        bool partially_filled;
        int partial_volume;
    public:
        LimitOrder* next;
        LimitOrder* prev;
        
        Limit* parent_limit;
        
        // constructor
        LimitOrder(string order_ticker, int order_volume, float order_price, orderSide order_side, int order_id, std::chrono::system_clock::time_point order_time) {
            ticker = std::move(order_ticker);
            price = order_price;
            volume = order_volume;
            side = order_side;
            
            id = order_id;
            time = order_time;
            
            partially_filled = false;
            partial_volume = order_volume;
            
            next = nullptr;
            prev = nullptr;
            
            parent_limit = nullptr;
        }
        
        float get_price() const {
            return price;
        }
        
        int get_partial_volume() const {
            return partial_volume;
        }
        
        void partial_fill(int fill_volume) {
            partially_filled = true;
            partial_volume -= fill_volume;
        }
        
        bool is_partially_filled() {
            return partially_filled;
        }
        
        
    };
    
    class MarketOrder : public Order {
    public:
        // constructor
        MarketOrder(string order_ticker, int order_volume, orderSide order_side, int order_id, std::chrono::system_clock::time_point order_time) {
            ticker = std::move(order_ticker);
            volume = order_volume;
            side = order_side;
            
            id = order_id;
            time = order_time;
        }
    };
    
    class Limit {
        friend class Orderbook;
        friend class Exchange;
    private:
        float price;
        int limit_orders;
        int limit_volume;
    
        LimitOrder* head;
        LimitOrder* tail;
        
        Orderbook* parent_orderbook;
    public:
        // constructor
        Limit() : price(0), limit_orders(0), limit_volume(0), head(nullptr), tail(nullptr), parent_orderbook(nullptr) {}
        
        bool is_empty() {
            return head == nullptr && tail == nullptr;
        }
        
        float get_price() const {
            return price;
        }
        
        int get_volume() const {
            return limit_volume;
        }
        
        LimitOrder* get_head_order() const {
            return head;
        }
        
        void add_order(LimitOrder* new_order) {
            // new order at empty price level
            if (is_empty()) {
                head = new_order;
                tail = new_order;
                price = new_order->get_price();
            }
            // append to tail of list
            else {
                LimitOrder* tail_order = tail;
                new_order->prev = tail_order;
                tail_order->next = new_order;
                tail = new_order;
            }
            
            new_order->parent_limit = this;
            
            limit_orders++;
            limit_volume += new_order->get_volume();
        }
        
        void cancel_order(LimitOrder* this_order) {
            // order in middle of list
            if (this_order->prev != nullptr && this_order->next != nullptr) {
                this_order->prev->next = this_order->next;
                this_order->next->prev = this_order->prev;
            }
            // order at beginning of list
            else if (this_order->prev == nullptr && this_order->next != nullptr) {
                this_order->next->prev = nullptr;
            }
            // order at end of list
            else if (this_order->prev != nullptr && this_order->next == nullptr) {
                this_order->prev->next = nullptr;
            }
            
            // reset head and tail
            if (head == this_order && tail == this_order) {
                head = nullptr;
                tail = nullptr;
            }
            // order at head
            else if (head == this_order) {
                head = this_order->next;
            }
            // order at tail
            else if (tail == this_order) {
                tail = this_order->prev;
            }
            
            limit_orders--;
            limit_volume -= this_order->get_volume();
            
            delete this_order;
        }
        
        void change_order(LimitOrder* this_order) {
            cout << "change order limit" << endl;
        }
        
        bool limit_fulfill_order(unordered_map<int, LimitOrder*> &orders_map, int order_volume, float &spread) {
            
            bool head_side = head->get_side();

            while (order_volume > 0) {
                LimitOrder* this_order = head;
                
                // head order is fulfilled
                if (this_order->get_partial_volume() <= order_volume) {
                    // print order information when fulfilled
                    cout << "order " << this_order->get_id() << " executed at $" << this_order->get_price() << " per share:" << endl;
                    cout << "\t order id: " << this_order->get_id() << endl;
                    cout << "\t order ticker: " << this_order->get_ticker() << endl;
                    string side = (this_order->get_side() == buy) ? "buy" : "sell";
                    cout << "\t order side: " << side << endl;
                    cout << "\t order price: " << this_order->get_price() << endl;
                    cout << "\t order volume: " << this_order->get_volume() << endl;
                    
                    time_t tt = std::chrono::system_clock::to_time_t(this_order->get_time());
                    cout << "\t order time: " << ctime(&tt) << endl;
                    
                    // update spread
                    if (head_side == buy) {
                        spread += this_order->get_partial_volume() * this_order->get_price();
                    } else {
                        spread -= this_order->get_partial_volume() * this_order->get_price();
                    }
                    
                    // update appropriate volumes
                    order_volume -= this_order->get_partial_volume();
                    
                    limit_orders--;
                    limit_volume -= this_order->get_partial_volume();
                    
                    if (head_side == buy) {
                        this_order->parent_limit->parent_orderbook->bid_orders--;
                        this_order->parent_limit->parent_orderbook->bid_volume -= this_order->get_partial_volume();
                    } else {
                        this_order->parent_limit->parent_orderbook->ask_orders--;
                        this_order->parent_limit->parent_orderbook->ask_volume -= this_order->get_partial_volume();
                    }
                    
                    this_order->parent_limit->parent_orderbook->orderbook_orders--;
                    this_order->parent_limit->parent_orderbook->orderbook_volume -= this_order->get_partial_volume();

                    this_order->parent_limit->parent_orderbook->parent_exchange->total_num_orders--;
                    this_order->parent_limit->parent_orderbook->parent_exchange->total_volume -= this_order->get_partial_volume();
                    
                    // erase order
                    orders_map.erase(this_order->get_id());
                    
                    // check if order was only one at limit
                    if (head == this_order && tail == this_order) {
                        head = nullptr;
                        tail = nullptr;
                    } else {
                        head = this_order->next;
                        this_order->next->prev = nullptr;
                    }
                    
                    delete this_order;
                }
                // head order is partially filled
                else {
                    this_order->partial_fill(order_volume);
                    
                    // update appropriate volumes
                    limit_volume -= order_volume;
                                        
                    if (head_side == buy) {
                        this_order->parent_limit->parent_orderbook->bid_volume -= order_volume;
                    } else {
                        this_order->parent_limit->parent_orderbook->ask_volume -= order_volume;
                    }
                    
                    this_order->parent_limit->parent_orderbook->orderbook_volume -= order_volume;

                    this_order->parent_limit->parent_orderbook->parent_exchange->total_volume -= this_order->get_partial_volume();
                                    
                    // update spread
                    if (head_side == buy) {
                        spread += (this_order->get_volume() - this_order->get_partial_volume()) * this_order->get_price();
                    } else {
                        spread -= (this_order->get_volume() - this_order->get_partial_volume()) * this_order->get_price();
                    }
                    
                    order_volume = 0;
                }
            }
            
            return limit_volume == 0;
        }
        
        bool market_fulfill_order(unordered_map<int, LimitOrder*> &orders_map, int &order_volume, float &avg_price) {
            bool head_side = head->get_side();

            while (order_volume > 0 && limit_volume > 0) {
                LimitOrder* this_order = head;
                
                // head order is fulfilled
                if (this_order->get_partial_volume() <= order_volume) {
                    // print order information when fulfilled
                    cout << "order " << this_order->get_id() << " executed at $" << this_order->get_price() << " per share:" << endl;
                    cout << "\t order id: " << this_order->get_id() << endl;
                    cout << "\t order ticker: " << this_order->get_ticker() << endl;
                    string side = (this_order->get_side() == buy) ? "buy" : "sell";
                    cout << "\t order side: " << side << endl;
                    cout << "\t order price: " << this_order->get_price() << endl;
                    cout << "\t order volume: " << this_order->get_volume() << endl;
                    
                    time_t tt = std::chrono::system_clock::to_time_t(this_order->get_time());
                    cout << "\t order time: " << ctime(&tt) << endl;

                    // execution time?
                    
        
                    // calculate average market price
                    avg_price += this_order->get_partial_volume() * this_order->get_price();
                    
                    // update appropriate volumes
                    order_volume -= this_order->get_partial_volume();
                    
                    limit_orders--;
                    limit_volume -= this_order->get_partial_volume();
                    
                    if (head_side == buy) {
                        this_order->parent_limit->parent_orderbook->bid_orders--;
                        this_order->parent_limit->parent_orderbook->bid_volume -= this_order->get_partial_volume();
                    } else {
                        this_order->parent_limit->parent_orderbook->ask_orders--;
                        this_order->parent_limit->parent_orderbook->ask_volume -= this_order->get_partial_volume();
                    }
                    
                    this_order->parent_limit->parent_orderbook->orderbook_orders--;
                    this_order->parent_limit->parent_orderbook->orderbook_volume -= this_order->get_partial_volume();

                    this_order->parent_limit->parent_orderbook->parent_exchange->total_num_orders--;
                    this_order->parent_limit->parent_orderbook->parent_exchange->total_volume -= this_order->get_partial_volume();
                    
                    // erase order
                    orders_map.erase(this_order->get_id());
                    
                    // check if order was only one at limit
                    if (head == this_order && tail == this_order) {
                        head = nullptr;
                        tail = nullptr;
                    } else {
                        head = this_order->next;
                        this_order->next->prev = nullptr;
                    }
                    
                    delete this_order;
                }
                // head order is partially filled
                else {
                    this_order->partial_fill(order_volume);
                    
                    // update appropriate volumes
                    limit_volume -= order_volume;
                    
                    if (head_side == buy) {
                        this_order->parent_limit->parent_orderbook->bid_volume -= order_volume;
                    } else {
                        this_order->parent_limit->parent_orderbook->ask_volume -= order_volume;
                    }
                    
                    this_order->parent_limit->parent_orderbook->orderbook_volume -= order_volume;

                    this_order->parent_limit->parent_orderbook->parent_exchange->total_volume -= order_volume;

                    // calculate average market price
                    avg_price += order_volume * this_order->get_price();
                    
                    order_volume = 0;
                }
            }
            
            return limit_volume == 0;
        }
        
    };
    
    
    class Orderbook {
        friend class Limit;
        friend class Exchange;
    private:
        // binary trees of bid and ask limits
        map<float, Limit, greater<>> bids;
        map<float, Limit> asks;
        
        // hash tables of bid and ask limits
        unordered_map<float, Limit*> bids_table;
        unordered_map<float, Limit*> asks_table;
        
        // queue of incoming market orders
        queue<MarketOrder*> market_bids;
        queue<MarketOrder*> market_asks;
        
        const string ticker;
        
        int bid_orders;
        int bid_volume;
        
        int ask_orders;
        int ask_volume;
        
        int orderbook_orders;
        int orderbook_volume;
        
        Exchange* parent_exchange;
        
    public:
        // constructor
        Orderbook(string orderbook_ticker) : ticker(move(orderbook_ticker)), bid_orders(0), bid_volume(0), ask_orders(0), ask_volume(0), orderbook_orders(0), orderbook_volume(0), parent_exchange(nullptr) {}
        
        int get_num_orders() const {
            return 0;
        }
        
        int get_best_bid() const {
            return bids.begin()->second.get_price();
        }
        
        int get_best_ask() const {
            return asks.begin()->second.get_price();
        }
        
        void add_market_order(MarketOrder* new_order) {
            // bid order
            if (new_order->get_side() == buy) {
                market_bids.push(new_order);
            }
            // ask order
            else {
                market_asks.push(new_order);
            }
            
            orderbook_orders++;
            orderbook_volume += new_order->get_volume();
        }
        
        void add_limit_order(LimitOrder* new_order) {
            // bid order
            if (new_order->get_side() == buy) {
                // limit does not exist yet
                if (!bids_table.contains(new_order->get_price())) {
                    bids_table[new_order->get_price()] = &bids[new_order->get_price()];
                    bids_table[new_order->get_price()]->parent_orderbook = this;
                }
                
                bids_table[new_order->get_price()]->add_order(new_order);
                
                bid_orders++;
                bid_volume += new_order->get_volume();
            }
            // ask order
            else if (new_order->get_side() == sell) {
                // limit does not exist yet
                if (!asks_table.contains(new_order->get_price())) {
                    asks_table[new_order->get_price()] = &asks[new_order->get_price()];
                    asks_table[new_order->get_price()]->parent_orderbook = this;
                }
                
                asks_table[new_order->get_price()]->add_order(new_order);
                
                ask_orders++;
                ask_volume += new_order->get_volume();
            }
            
            orderbook_orders++;
            orderbook_volume += new_order->get_volume();
        }
        
        void cancel_order(LimitOrder* this_order) {
            // bid order
            if (this_order->get_side() == buy) {
                bids_table[this_order->get_price()]->cancel_order(this_order);
                
                if (bids_table[this_order->get_price()]->is_empty()) {
                    bids.erase(this_order->get_price());
                    bids_table.erase(this_order->get_price());
                }
                
                bid_orders--;
                bid_volume -= this_order->get_volume();
            }
            // asks order
            else if (this_order->get_side() == sell) {
                asks_table[this_order->get_price()]->cancel_order(this_order);
                
                if (asks_table[this_order->get_price()]->is_empty()) {
                    asks.erase(this_order->get_price());
                    asks_table.erase(this_order->get_price());
                }
                
                ask_orders--;
                ask_volume -= this_order->get_volume();
            }
            
            orderbook_orders--;
            orderbook_volume -= this_order->get_volume();
        }
        
        void change_order(Order* this_order) {
            cout << "change order orderbook" << endl;
        }
        
        float match_limit_orders(unordered_map<int, LimitOrder*> &orders_map) {
            // no bid or ask orders
            if (bids.empty() || asks.empty()) {
                return 0;
            }
            
            auto bids_it = bids.begin();
            auto asks_it = asks.begin();
            
            float spread = 0;

            // fulfill limit orders when lowest asks is less than highest bid
            while (get_best_ask() <= get_best_bid()) {
                cout << "start matching orders" << endl;
                
                int volume_to_fulfill = min(bids_it->second.get_volume(), asks_it->second.get_volume());
                
                // need to reset best bid
                if (bids_it->second.limit_fulfill_order(orders_map, volume_to_fulfill, spread)) {
                    bids_table.erase(bids_it->second.get_price());
                    bids.erase(bids_it);
                    bids_it = bids.begin();
                }
                
                // need to reset best ask
                if (asks_it->second.limit_fulfill_order(orders_map, volume_to_fulfill, spread)) {
                    asks_table.erase(asks_it->second.get_price());
                    asks.erase(asks_it);
                    asks_it = asks.begin();
                }
            }
            
            cout << "done with limit orders" << endl;

            return spread;
        }
        
        void market_match_orders(unordered_map<int, LimitOrder*> &orders_map) {
            // edge cases
            if (market_bids.empty() && market_asks.empty()) {
                return;
            } else if (market_bids.empty() && (!market_asks.empty() && market_asks.front()->get_volume() > bid_volume)) {
                return;
            } else if (market_asks.empty() && (!market_bids.empty() && market_bids.front()->get_volume() > ask_volume)) {
                return;
            } else if ((!market_bids.empty() && market_bids.front()->get_volume() > ask_volume) && (!market_asks.empty() && market_asks.front()->get_volume() > bid_volume)) {
                return;
            }
                        
            auto bids_it = bids.begin();
            auto asks_it = asks.begin();
            
            bool no_valid_bid_orders = false;
            bool no_valid_ask_orders = false;
            
            while (!market_bids.empty() || !market_asks.empty()) {
                
                if (market_bids.empty() || market_bids.front()->get_volume() > ask_volume) no_valid_bid_orders = true;
                if (market_asks.empty() || market_asks.front()->get_volume() > bid_volume) no_valid_ask_orders = true;
                
                MarketOrder* this_order;
                
                // no valid market orders
                if (no_valid_bid_orders && no_valid_ask_orders) break;
                // only ask orders valid
                else if (no_valid_bid_orders) this_order = market_asks.front();
                // only bid orders valid
                else if (no_valid_ask_orders) this_order = market_bids.front();
                // get first market order in queue
                else this_order = (market_bids.front()->get_time() < market_asks.front()->get_time()) ? market_bids.front() : market_asks.front();
                
                int this_order_volume = this_order->get_volume();
                float avg_price = 0;
                
                // buy market order
                if (this_order->get_side() == buy) {
                    while (this_order_volume > 0) {
                        if (asks_it->second.market_fulfill_order(orders_map, this_order_volume, avg_price)) {
                            asks_table.erase(asks_it->second.get_price());
                            asks.erase(asks_it);
                            asks_it = asks.begin();
                        }
                    }
                }
                // sell market order
                else if (this_order->get_side() == sell) {
                    while (this_order_volume > 0) {
                        if (bids_it->second.market_fulfill_order(orders_map, this_order_volume, avg_price)) {
                            bids_table.erase(bids_it->second.get_price());
                            bids.erase(bids_it);
                            bids_it = bids.begin();
                        }
                    }
                }
                
                avg_price /= this_order->get_volume();
                
                cout << "order " << this_order->get_id() << " executed at $" << avg_price << " per share:" << endl;
                cout << "\t order id: " << this_order->get_id() << endl;
                cout << "\t order ticker: " << this_order->get_ticker() << endl;
                string side = (this_order->get_side() == buy) ? "buy" : "sell";
                cout << "\t order side: " << side << endl;
                cout << "\t order volume: " << this_order->get_volume() << endl;
                
                time_t tt = std::chrono::system_clock::to_time_t(this_order->get_time());
                cout << "\t order time: " << ctime(&tt) << endl;
                
                this_order->get_side() == buy ? market_bids.pop() : market_asks.pop();
                
                orderbook_orders--;
                orderbook_volume -= this_order->get_volume();
                
                this->parent_exchange->total_num_orders--;
                this->parent_exchange->total_volume -= this_order->get_volume();
            }
            
            cout << "done with market orders" << endl;
        }
    };
    
private:
    // hash table of all orderbooks
    unordered_map<string, Orderbook*> orderbooks;
    
    // hash table of all orders
    unordered_map<int, LimitOrder*> orders;
    
    float total_captured_spread;
    int total_num_orders;
    int total_volume;
    
public:
    // constructor
    Exchange() : total_captured_spread(0), total_num_orders(0), total_volume(0) {}
    
    int add_order(orderType order_type, string order_ticker, float order_price, int order_volume, orderSide order_side) {
        // set random id and current time
        int order_id = rand() % 1000;
        std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
        
        // orderbook for new ticker
        if (!orderbooks.contains(order_ticker)) {
            auto* new_orderbook = new Orderbook(order_ticker);
            new_orderbook->parent_exchange = this;
            
            orderbooks.insert({order_ticker, new_orderbook});
        }

        // create new market order
        if (order_type == market) {
            auto* new_order = new MarketOrder(order_ticker, order_volume, order_side, order_id, current_time);
            orderbooks.at(order_ticker)->add_market_order(new_order);
        }
        // create and process new limit order
        else {
            auto* new_order = new LimitOrder(order_ticker, order_volume, order_price, order_side, order_id, current_time);
            orderbooks.at(order_ticker)->add_limit_order(new_order);
            orders[order_id] = new_order;
        }
        
        total_num_orders++;
        total_volume += order_volume;
        
        execute();
        
        cout << "order id: " << order_id << endl;
        return order_id;
    }
    
    void cancel_order(int order_id) {
        // order exists
        if (orders.contains(order_id)) {
            LimitOrder* this_order = orders[order_id];
            
            orderbooks.at(this_order->get_ticker())->cancel_order(this_order);
            
            orders.erase(order_id);
            
            total_num_orders--;
            total_volume -= this_order->get_volume();
            
            cout << "order id " << order_id << " canceled" << endl;
        }
        // order does not exist
        else {
            cerr << "order does not exist" << endl;
        }
    }
    
    void change_order(int order_id) {
        cout << "change order exchange" << endl;
        
        // execute();
    }
    
    void execute() {
        for (auto orderbook : orderbooks) {
            orderbook.second->market_match_orders(orders);
            total_captured_spread += orderbook.second->match_limit_orders(orders);
        }
        
        cout << "execute complete" << endl;
    }
    
    void print_order(int order_id) {
        // order exists
        if (orders.contains(order_id)) {
            LimitOrder* this_order = orders[order_id];
            
            cout << "order id: " << order_id << endl;
            cout << "order ticker: " << this_order->get_ticker() << endl;
            string side = (this_order->get_side() == buy) ? "buy" : "sell";
            cout << "order side: " << side << endl;
            cout << "order price: " << this_order->get_price() << endl;
            cout << "order volume: " << this_order->get_volume() << endl;
            
            time_t tt = std::chrono::system_clock::to_time_t(this_order->get_time());
            cout << "order time: " << ctime(&tt) << endl;
        }
        // order does not exist
        else {
            cerr << "order does not exist" << endl;
        }
    }
    
    LimitOrder* find_order(int order_id) {
        return orders[order_id];
    }
};


int main(int argc, const char * argv[]) {
    srand((unsigned)time(nullptr));
    
    cout << "lets go again" << endl;
    
    Exchange exchange;
    
    
    // i want to add a limit order for apple at $100, 100 shares, buy
    
    
    // exchange.add_order(limit, "APPL", 100, 100, buy);
 
    
    
    exchange.add_order(market, "APPL", 0, 40, buy);
    exchange.add_order(market, "APPL", 0, 100, sell);
    
    int order1, order2, order3, order4, order5, order6, order7;
    
    order1 = exchange.add_order(limit, "APPL", 100, 100, buy);
    order2 = exchange.add_order(limit, "APPL", 99, 150, buy);
    order3 = exchange.add_order(limit, "APPL", 98, 130, buy);

    order4 = exchange.add_order(limit, "APPL", 101, 100, sell);
    order7 = exchange.add_order(limit, "APPL", 101, 50, sell);
    order5 = exchange.add_order(limit, "APPL", 102, 150, sell);
    order6 = exchange.add_order(limit, "APPL", 103, 130, sell);
    
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
    exchange.add_order(limit, "APPL", 102, 120, buy);
    
    exchange.print_order(order7);
    
    // should fulfill all of order 4, then leave with 20 more volume on buy side
    
    cout << "finished" << endl;
    
    return 0;
}
