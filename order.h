#ifndef ORDER_H
#define ORDER_H

#include<bits/stdc++.h>
#include <fstream>
using namespace std;
using ll = long long;
class Order {
public:
    // Optimized class for efficiency
    int sno;
    string ts_event;
    int rtype;
    int publisher_id;
    int instrument_id;
    char action;
    char side;
    ll price; // price = actual price * 1e9
    int size;
    int order_id;
    int flags;
    int ts_in_delta;
    int sequence;
    string symbol;

    Order();

    Order(int sno, string ts_ev, int rtyp, int pub_id, int inst_id, char actn, char sd, ll prc, int sz, int o_id, int flgs, int ts_del, int seq, string sym);

    void parse_row(string& row, int row_no);

    void write_row(ofstream& fileStream, int depth);
};

class MBPK {
public:
    // Need to maintain top-10 bid and ask prices
    // Will process each order row by row
    // If an order makes a change in the top-10 , then we should display it in the mbp-10 file
    // else we should just store it
    int k;
    list<ll> bid, ask; // these will store the topk
    priority_queue<ll> bidq; // these will store the rest (max-heap by default)
    priority_queue<ll, vector<ll>, greater<ll> > askq;
    unordered_map<ll, bool> removed_bid, removed_ask; // for lazy removals of orders
    unordered_map<ll, bool> inBidq, inAskq;
    bool change; // if the current row makes a change in the top-10 then we will print it to mbp10, else not
    unordered_map<ll, unordered_map<ll, int> > bid_to_order, ask_to_order;
    // price_to_order[price] = unordered_map<int,int> where order_id will be a key and the quantity will be a value
    int sno;

    MBPK(int top);

    void displayOrder(Order& order, ofstream& fout, int depth);

    void processOrder(Order& order, ofstream& fout);

    list<ll>::iterator find_bid(ll price, int& depth);
    list<ll>::iterator find_ask(ll price, int& depth);

    void reset();
};

#endif
