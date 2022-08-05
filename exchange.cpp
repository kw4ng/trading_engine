#include <iostream>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <vector>
#include <queue>
#include <thread>

using namespace std;

/*
 
    ORDERBOOK FEATURES:
        - process limit and market orders
        - add, cancel, and change order functions
        - FIFO w/ partially fufilled orders (maybe add PRORATA)
        - acting as market maker to capture spread
 
 */

/*

 TODO:

 - add market orders
 - change order function
 - execute function
 - server/client application
 - multithreading

*/


/*
 
 HOW ORDERS ARE FUFILLED
 
 
 - IF SELL ORDER SITS ON BOOK AT 99, AND BUY ORDER COMES IN FOR 101
    - ORDER FULFILLED AT 99, SELLER GETS 99 AND BUY GETS 99, MARKET MAKER GETS 0
 
 - IF BUY ORDER SITS ON BOOK AT 101, AND SELL ORDER COMES IN FOR 99
    - ORDER FULFILLED AT 101, BUYER BUYERS FOR 101 AND SELLER SELLS FOR 101
 
 
 - IF SELL ORDER SITS ON BOOK AT 99 AND 98, AND BUY ORDER COMES IN FOR 101
    - ORDER FUFILLED AT 98, SELLER GETS 99 AND BUYER GETS 98, MARKET MAKER GETS 0
 
 
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
private:
    
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
        
        void partial_fill(int fill_volume) {
            volume -= fill_volume;
        }
    };
    
    class LimitOrder : public Order {
    public:
        float price;
    public:
        LimitOrder* next;
        LimitOrder* prev;
        
        // constructor
        LimitOrder(string order_ticker, int order_volume, float order_price, orderSide order_side, int order_id, std::chrono::system_clock::time_point order_time) {
            ticker = order_ticker;
            price = order_price;
            volume = order_volume;
            side = order_side;
            
            id = order_id;
            time = order_time;
            
            next = nullptr;
            prev = nullptr;
        }
        
        virtual float get_price() {
            return price;
        }
    };
    
    class MarketOrder : public Order {
    public:
        // constructor
        MarketOrder(string order_ticker, int order_volume, orderSide order_side, int order_id, std::chrono::system_clock::time_point order_time) {
            ticker = order_ticker;
            volume = order_volume;
            side = order_side;
            
            id = order_id;
            time = order_time;
        }
    };
    
    
    
    class Limit {
    private:
        float price;
        int limit_orders;
        int limit_volume;
        
        LimitOrder* head;
        LimitOrder* tail;
        
    public:
        // constructor
        Limit() : price(0), limit_orders(0), limit_volume(0), head(nullptr), tail(nullptr) {}
        
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
        
        void fulfill_head_order(unordered_map<int, LimitOrder*> &orders_map, int order_volume) {
            // head order is fulfilled
            if (head->get_volume() <= order_volume) {
                cout << "order " << head->get_id() << " executed at $" << head->get_price() << " per share" << endl;

                limit_orders--;
                limit_volume -= head->get_volume();

                // set new head to next
                LimitOrder* new_head = head->next;
                delete head;
                head = new_head;

            }
            // head order only partially filled
            else {
                head->partial_fill(order_volume);
                limit_volume -= order_volume;
            }

            
            
            
            
//            vector<int> fulfilled_orders;
//
//            while (order_volume > 0) {
//                // order at head is fulfilled
//                if (head->get_volume() <= order_volume) {
//                    order_volume -= head->get_volume();
//
//                    fulfilled_orders.push_back(head->get_id());
//
//                    limit_orders--;
//                    limit_volume -= head->get_volume();
//
//                    // set new head to next
//                    LimitOrder* new_head = head->next;
//                    delete head;
//                    head = new_head;
//                }
//                // head volume covers entire volume
//                else {
//                    head->partial_fill(order_volume);
//                    limit_volume -= order_volume;
//                    order_volume = 0;
//                }
//            }
        }
    };
    
    
    class Orderbook {
    private:
        // binary trees of bid and ask limits
        map<float, Limit, greater<>> bids;
        map<float, Limit> asks;
        
        // hash tables of bid and ask limits
        unordered_map<float, Limit*> bids_table;
        unordered_map<float, Limit*> asks_table;
        
        // queue of incoming market orders
        queue<MarketOrder*> market_orders;
        
        const string ticker;
        
        int orderbook_orders;
        int orderbook_volume;
        
    public:
        // constructor
        Orderbook(string orderbook_ticker) : ticker(move(orderbook_ticker)), orderbook_orders(0), orderbook_volume(0) {}
        
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
            market_orders.push(new_order);
        }
        
        void add_limit_order(LimitOrder* new_order) {
            // bid order
            if (new_order->get_side() == buy) {
                // limit does not exist yet
                if (!bids_table.contains(new_order->price)) {
                    bids_table[new_order->price] = &bids[new_order->price];
                }
                
                bids_table[new_order->price]->add_order(new_order);
            }
            // ask order
            else if (new_order->get_side() == sell) {
                // limit does not exist yet
                if (!asks_table.contains(new_order->price)) {
                    asks_table[new_order->price] = &asks[new_order->price];
                }
                
                asks_table[new_order->price]->add_order(new_order);
            }
            
            orderbook_orders++;
            orderbook_volume += new_order->get_volume();
        }
        
        void cancel_order(LimitOrder* this_order) {
            // bid order
            if (this_order->get_side() == buy) {
                bids_table[this_order->price]->cancel_order(this_order);
                
                if (bids_table[this_order->price]->is_empty()) {
                    bids.erase(this_order->price);
                    bids_table.erase(this_order->price);
                }
            }
            // asks order
            else if (this_order->get_side() == sell) {
                asks_table[this_order->price]->cancel_order(this_order);
                
                if (asks_table[this_order->price]->is_empty()) {
                    asks.erase(this_order->price);
                    asks_table.erase(this_order->price);
                }
            }
            
            orderbook_orders--;
            orderbook_volume -= this_order->get_volume();
        }
        
        void change_order(Order* this_order) {
            cout << "change order orderbook" << endl;
        }
        
        int match_orders(unordered_map<int, LimitOrder*> &orders_map) {
            // no bid or no ask orders
            if (bids.empty() || asks.empty()) {
                return 0;
            }
            
            auto bids_it = bids.begin();
            auto asks_it = asks.begin();
            
            // fulfill market orders
//            if (!market_orders.empty()) {
//                MarketOrder* current_order = market_orders.front();
//                // buy order
//                if (current_order->get_side() == buy) {
//                    while (current_order->get_volume() > 0) {
//                        current_order->partial_fill(asks_it->second.fulfill_top_order(orders_map, current_order->get_id(), current_order->get_volume()));
//                    }
//
//
//
//                    cout << "order " << current_order->get_id() << " executed at $ per share" << endl;
//
//                    market_orders.pop();
//                    delete current_order;
//                }
//                // sell order
//                else {
//                    // bids_it->second.fulfill_top_order(market_orders.front()->get_volume());
//                }
//            }
            
            float spread = 0;

            // fulfill orders when lowest asks is less than highest bid
            while (get_best_ask() <= get_best_bid()) {
                cout << "start matching orders" << endl;
                
                int volume_to_fulfill = min(bids_it->second.get_volume(), asks_it->second.get_volume());
                
                while (volume_to_fulfill > 0) {
                    // bid order came first
                    if (bids_it->second.get_head_order()->get_time() < asks_it->second.get_head_order()->get_time()) {
                        bids_it->second.fulfill_head_order(orders_map, volume_to_fulfill);
                        asks_it->second.fulfill_head_order(orders_map, volume_to_fulfill);
                    }
                    // ask order came first
                    else {
                        asks_it->second.fulfill_head_order(orders_map, volume_to_fulfill);
                        bids_it->second.fulfill_head_order(orders_map, volume_to_fulfill);
                    }
                    
                }
            }
            
            return spread;
        }
    };
    
private:
    // hash table of all orderbooks
    unordered_map<string, Orderbook> orderbooks;
    
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
            Orderbook new_orderbook(order_ticker);
            orderbooks.insert({order_ticker, new_orderbook});
        }

        // create new market order
        if (order_type == market) {
            MarketOrder* new_order = new MarketOrder(order_ticker, order_volume, order_side, order_id, current_time);
            orderbooks.at(order_ticker).add_market_order(new_order);
        }
        // create and process new limit order
        else {
            LimitOrder* new_order = new LimitOrder(order_ticker, order_volume, order_price, order_side, order_id, current_time);
            orderbooks.at(order_ticker).add_limit_order(new_order);
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
            
            orderbooks.at(this_order->get_ticker()).cancel_order(this_order);
            
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
            total_captured_spread += orderbook.second.match_orders(orders);
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
 
    
    
    
    
    int order1, order2, order3, order4, order5, order6;
    
    order1 = exchange.add_order(limit, "APPL", 100, 100, buy);
    order2 = exchange.add_order(limit, "APPL", 99, 150, buy);
    order3 = exchange.add_order(limit, "APPL", 98, 130, buy);

    order4 = exchange.add_order(limit, "APPL", 101, 100, sell);
    exchange.add_order(limit, "APPL", 101, 50, sell);
    order5 = exchange.add_order(limit, "APPL", 102, 150, sell);
    order6 = exchange.add_order(limit, "APPL", 103, 130, sell);
    
//    exchange.print_order(order1);
//    exchange.print_order(order2);
//    exchange.print_order(order3);
//    exchange.print_order(order4);
//    exchange.print_order(order5);
//    exchange.print_order(order6);
        
    cout << "adding matchable orders" << endl;
    
    exchange.add_order(limit, "APPL", 101, 50, buy);
    
    // should fulfill all of order 4, then leave with 20 more volume on buy side
    
    cout << "finished" << endl;
    
    return 0;
}
