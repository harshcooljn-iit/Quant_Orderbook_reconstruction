#include "order.h"

Order::Order() {
    rtype = 0;
    publisher_id = 0;
    instrument_id = 0;
    price = -1; size = 0; order_id = 0;
    flags = 0; ts_in_delta = 0; sequence = -1;
}

Order::Order(int s_no, string ts_ev, int rtyp, int pub_id, int inst_id, char actn, char sd, ll prc, int sz, int o_id, int flgs, int ts_del, int seq, string sym) {
    sno = s_no;
    ts_event = ts_ev;
    rtype = rtyp;
    publisher_id = pub_id;
    instrument_id = inst_id;
    action = actn;
    side = sd;
    price = prc;
    size = sz;
    order_id = o_id;
    flags = flgs;
    ts_in_delta = ts_del;
    sequence = seq;
    symbol = sym;
}

void Order::parse_row(string& row, int row_no) {
    sno = row_no;
    ts_event = row.substr(31, 30);
    rtype = 0;
    publisher_id = 0;
    instrument_id = 0;
    price = -1; size = 0; order_id = 0;
    flags = 0; ts_in_delta = 0; sequence = 0;
    short int comma_cnt = 0;
    for (int i = 62; i < (int)row.size(); i++) {
        if (comma_cnt == 0) {
            while (i < (int)row.size() && row[i] != ',') {
                rtype *= 10;
                rtype += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 1) {
            while (i < (int)row.size() && row[i] != ',') {
                publisher_id *= 10;
                publisher_id += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 2) {
            while (i < (int)row.size() && row[i] != ',') {
                instrument_id *= 10;
                instrument_id += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 3) {
            action = row[i];
            i++;
            comma_cnt++;
        }
        else if (comma_cnt == 4) {
            side = row[i];
            i++;
            comma_cnt++;
        }
        else if (comma_cnt == 5) {
            if (action != 'R') price = 0;
            while (i < (int)row.size() && row[i] != ',') {
                if (row[i] == '.') {
                    i++;
                    continue;
                }
                price *= 10;
                price += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 6) {
            while (i < (int)row.size() && row[i] != ',') {
                size *= 10;
                size += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 7) {
            while (i < (int)row.size() && row[i] != ',') {
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 8) {
            while (i < (int)row.size() && row[i] != ',') {
                order_id *= 10;
                order_id += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 9) {
            while (i < (int)row.size() && row[i] != ',') {
                flags *= 10;
                flags += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 10) {
            while (i < (int)row.size() && row[i] != ',') {
                ts_in_delta *= 10;
                ts_in_delta += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else if (comma_cnt == 11) {
            while (i < (int)row.size() && row[i] != ',') {
                sequence *= 10;
                sequence += (row[i] - '0');
                i++;
            }
            comma_cnt++;
        }
        else {
            symbol.clear();
            while (i < (int)row.size()) {
                symbol.push_back(row[i]);
                i++;
            }
        }
    }
}

void Order::write_row(ofstream& fout, int depth) {
    fout << sno << ',';
    fout << ts_event << ',' << ts_event << ',';
    fout << rtype << ',' << publisher_id << ',' << instrument_id << ',' << action << ',' << side << ',' << depth << ',';
    if (price != -1) {
        double p = price * 1e-9;
        fout << p;
    }
    fout << ',' << size << ',' << flags << ',' << ts_in_delta << ',';
    if (sequence != -1) {
        fout << sequence;
    }
    fout << ',';
    // fout << ",0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,,0,0,"; // just printint for now , delete later
    // fout << symbol << ',' << order_id << "\n";
}

MBPK::MBPK(int top) {
    k = top;
    sno = 0;
}

void MBPK::displayOrder(Order& order, ofstream& fout, int depth) {
    // print all order details
    order.sno = sno++;
    order.write_row(fout, depth);

    // print topk details
    auto bid_it = bid.begin();
    auto ask_it = ask.begin();

    for (int i = 0;i < 10;i++) {
        // bid details
        if (!bid.empty() && bid_it != bid.end()) {
            ll price = *bid_it;
            // cout << bid.size() << endl;
            // cout << price << endl;
            double p = price * 1e-9;
            fout << p << ',' << bid_to_order[price][-1] << ',' << bid_to_order[price].size() - 1 << ',';
            bid_it++;
        }
        else {
            fout << ",0,0,";
        }

        // ask details
        if (!ask.empty() && ask_it != ask.end()) {
            ll price = *ask_it;
            double p = price * 1e-9;
            fout << p << ',' << ask_to_order[price][-1] << ',' << ask_to_order[price].size() - 1 << ',';
            ask_it++;
        }
        else {
            fout << ",0,0,";
        }
    }

    fout << order.symbol << ',' << order.order_id << '\n';
}

list<ll>::iterator MBPK::find_bid(ll price, int& depth) {
    auto it = bid.begin();
    while (it != bid.end()) {
        if ((*it) > price) {
            it++;
            depth++;
        }
        else {
            break;
        }
    }
    return it;
}

list<ll>::iterator MBPK::find_ask(ll price, int& depth) {
    auto it = ask.begin();
    while (it != ask.end()) {
        if ((*it) < price) {
            it++;
            depth++;
        }
        else {
            break;
        }
    }
    return it;
}

void MBPK::processOrder(Order& order, ofstream& fout) {
    change = false;
    int depth = 0;
    if (order.action == 'A') {
        if (order.side == 'B') {
            if (bid.empty()) {
                bid.push_back(order.price);
                bid_to_order[order.price][order.order_id] += order.size;
                bid_to_order[order.price][-1] += order.size;
                change = true;
            }
            else if (bid.back() <= order.price) {
                auto it = find_bid(order.price, depth);
                change = true;
                if (it != bid.end() && (*it) == order.price) {
                    // node already present
                    bid_to_order[order.price][order.order_id] += order.size;
                    bid_to_order[order.price][-1] += order.size;
                }
                else {
                    // so (*it) < order.price
                    bid.insert(it, order.price);
                    bid_to_order[order.price][order.order_id] += order.size;
                    bid_to_order[order.price][-1] += order.size;

                    while ((int)bid.size() > k) {
                        if (inBidq.find(bid.back()) == inBidq.end() || inBidq[bid.back()] == false) {
                            inBidq[bid.back()] = true;
                            bidq.push(bid.back());
                            bid.pop_back();
                        }
                        else {
                            // already present in heap
                            bid.pop_back();
                        }
                    }
                }
            }
            else if (bid.back() > order.price && (int)bid.size() < k) {
                bid.push_back(order.price);
                bid_to_order[order.price][order.order_id] += order.size;
                bid_to_order[order.price][-1] += order.size;
                change = true;
                depth = bid.size() - 1;
            }
            else {
                if (removed_bid.find(order.price) != removed_bid.end()) {
                    if (removed_bid[order.price] == true && inBidq[order.price] == true) {
                        // so this bid had to be removed (lazy deletion but it hasnt been removed yet), so instead of pushing it again we can just use this node which is already present
                        removed_bid[order.price] = false; // now the bid is back in the order book
                        bid_to_order[order.price][order.order_id] += order.size;
                        bid_to_order[order.price][-1] += order.size;
                    }
                    else if (removed_bid[order.price] == false && inBidq[order.price] == true) {
                        bid_to_order[order.price][order.order_id] += order.size;
                        bid_to_order[order.price][-1] += order.size;
                    }
                    else if (removed_bid[order.price] == true && inBidq[order.price] == false) {
                        removed_bid[order.price] = false;
                        inBidq[order.price] = true;
                        bidq.push(order.price);
                        bid_to_order[order.price][order.order_id] += order.size;
                        bid_to_order[order.price][-1] += order.size;
                    }
                    else /*if (removed_bid[order.price] == false && inBidq[order.price] == false)*/ {
                        inBidq[order.price] = true;
                        bidq.push(order.price);
                        bid_to_order[order.price][order.order_id] += order.size;
                        bid_to_order[order.price][-1] += order.size;
                    }
                }
                else {
                    // this means bid.back() > order.price , so order.price is at higher depth than k
                    if (inBidq.find(order.price) == inBidq.end() || inBidq[order.price] == false) {
                        inBidq[order.price] = true;
                        bidq.push(order.price);
                    }
                    else {
                        // already present in heap
                    }
                    bid_to_order[order.price][order.order_id] += order.size;
                    bid_to_order[order.price][-1] += order.size;
                }
            }
        }
        else {
            if (ask.empty()) {
                ask.push_back(order.price);
                ask_to_order[order.price][order.order_id] += order.size;
                ask_to_order[order.price][-1] += order.size;
                change = true;
            }
            else if (ask.back() >= order.price) {
                auto it = find_ask(order.price, depth);
                change = true;
                if (it != ask.end() && (*it) == order.price) {
                    // node already present
                    ask_to_order[order.price][order.order_id] += order.size;
                    ask_to_order[order.price][-1] += order.size;
                }
                else {
                    // so (*it) > order.price
                    ask.insert(it, order.price);
                    ask_to_order[order.price][order.order_id] += order.size;
                    ask_to_order[order.price][-1] += order.size;

                    while ((int)ask.size() > k) {
                        if (inAskq.find(ask.back()) == inAskq.end() || inAskq[ask.back()] == false) {
                            inAskq[ask.back()] = true;
                            askq.push(ask.back());
                            ask.pop_back();
                        }
                        else {
                            // already present in heap
                            ask.pop_back();
                        }
                    }
                }
            }
            else if (ask.back() < order.price && (int)ask.size() < k) {
                ask.push_back(order.price);
                ask_to_order[order.price][order.order_id] += order.size;
                ask_to_order[order.price][-1] += order.size;
                change = true;
                depth = ask.size() - 1;
            }
            else {
                if (removed_ask.find(order.price) != removed_ask.end()) {
                    if (removed_ask[order.price] == true && inAskq[order.price] == true) {
                        // so this ask had to be removed (lazy deletion but it hasnt been removed yet), so instead of pushing it again we can just use this node which is already present
                        removed_ask[order.price] = false; // now the ask is back in the order book
                        ask_to_order[order.price][order.order_id] += order.size;
                        ask_to_order[order.price][-1] += order.size;
                    }
                    else if (removed_ask[order.price] == false && inAskq[order.price] == true) {
                        ask_to_order[order.price][order.order_id] += order.size;
                        ask_to_order[order.price][-1] += order.size;
                    }
                    else if (removed_ask[order.price] == true && inAskq[order.price] == false) {
                        removed_ask[order.price] = false;
                        inAskq[order.price] = true;
                        askq.push(order.price);
                        ask_to_order[order.price][order.order_id] += order.size;
                        ask_to_order[order.price][-1] += order.size;
                    }
                    else /*if (removed_ask[order.price] == false && inAskq[order.price] == false)*/ {
                        inAskq[order.price] = true;
                        askq.push(order.price);
                        ask_to_order[order.price][order.order_id] += order.size;
                        ask_to_order[order.price][-1] += order.size;
                    }
                }
                else {
                    // this means ask.back() < order.price , so order.price is at higher depth than k
                    if (inAskq.find(order.price) == inAskq.end() || inAskq[order.price] == false) {
                        inAskq[order.price] = true;
                        askq.push(order.price);
                    }
                    else {
                        // already present in heap
                    }
                    ask_to_order[order.price][order.order_id] += order.size;
                    ask_to_order[order.price][-1] += order.size;
                }
            }
        }
    }
    else if (order.action == 'C' || order.action == 'T') {
        if (order.side == 'B') {
            if (inBidq.find(order.price) != inBidq.end() && inBidq[order.price] == true) {
                // then this suboptimal price is in heap
                if (bid_to_order[order.price][order.order_id] == order.size) {
                    bid_to_order[order.price].erase(order.order_id);
                    if (bid_to_order[order.price][-1] == order.size) {
                        bid_to_order.erase(order.price);
                        removed_bid[order.price] = true; // lazy deletion
                    }
                    else {
                        bid_to_order[order.price][-1] -= order.size;
                    }
                }
                else {
                    bid_to_order[order.price][order.order_id] -= order.size;
                    bid_to_order[order.price][-1] -= order.size;
                }
            }
            else {
                // this order price is not in heap , so it may be in bid list
                auto it = find_bid(order.price, depth);
                if (it != bid.end() && (*it) == order.price) {
                    // this price is present in the list
                    change = true;
                    if (bid_to_order[order.price][order.order_id] == order.size) {
                        bid_to_order[order.price].erase(order.order_id);
                        if (bid_to_order[order.price][-1] == order.size) {
                            bid_to_order.erase(order.price);
                            // now we must delete it from the list
                            bid.erase(it);
                            // now if any order is available in the heap we must push it to the back of the list
                            // because we know that the back of the list is always > than the max element of heap
                            while (!bidq.empty()) {
                                ll price = bidq.top();
                                bidq.pop();
                                inBidq[price] = false;
                                if (removed_bid.find(price) != removed_bid.end()) {
                                    if (removed_bid[price] == false) {
                                        // then we can push this price to the back of the bid list
                                        bid.push_back(price);
                                        break;
                                    }
                                    else {
                                        continue;
                                    }
                                }
                                else {
                                    // so this bid was not cancelled
                                    bid.push_back(price);
                                    break;
                                }
                            }
                        }
                        else {
                            bid_to_order[order.price][-1] -= order.size;
                        }
                    }
                    else {
                        bid_to_order[order.price][order.order_id] -= order.size;
                        bid_to_order[order.price][-1] -= order.size;
                    }
                }
                else {
                    // invalid , since the order could not be found
                }
            }
        }
        else {
            if (inAskq.find(order.price) != inAskq.end() && inAskq[order.price] == true) {
                // then this suboptimal price is in heap
                if (ask_to_order[order.price][order.order_id] == order.size) {
                    ask_to_order[order.price].erase(order.order_id);
                    if (ask_to_order[order.price][-1] == order.size) {
                        ask_to_order.erase(order.price);
                        removed_ask[order.price] = true; // lazy deletion
                    }
                    else {
                        ask_to_order[order.price][-1] -= order.size;
                    }
                }
                else {
                    ask_to_order[order.price][order.order_id] -= order.size;
                    ask_to_order[order.price][-1] -= order.size;
                }
            }
            else {
                // this order price is not in heap , so it may be in ask list
                auto it = find_ask(order.price, depth);
                if (it != ask.end() && (*it) == order.price) {
                    // this price is present in the list
                    change = true;
                    if (ask_to_order[order.price][order.order_id] == order.size) {
                        ask_to_order[order.price].erase(order.order_id);
                        if (ask_to_order[order.price][-1] == order.size) {
                            ask_to_order.erase(order.price);
                            // now we must delete it from the list
                            ask.erase(it);
                            // now if any order is available in the heap we must push it to the back of the list
                            // because we know that the back of the list is always < than the min element of heap
                            while (!askq.empty()) {
                                ll price = askq.top();
                                askq.pop();
                                inAskq[price] = false;
                                if (removed_ask.find(price) != removed_ask.end()) {
                                    if (removed_ask[price] == false) {
                                        // then we can push this price to the back of the ask list
                                        ask.push_back(price);
                                        break;
                                    }
                                    else {
                                        continue;
                                    }
                                }
                                else {
                                    // so this ask was not cancelled
                                    ask.push_back(price);
                                    break;
                                }
                            }
                        }
                        else {
                            ask_to_order[order.price][-1] -= order.size;
                        }
                    }
                    else {
                        ask_to_order[order.price][order.order_id] -= order.size;
                        ask_to_order[order.price][-1] -= order.size;
                    }
                }
                else {
                    // invalid , since the order could not be found
                }
            }
        }

        if (order.action == 'T') {
            order.order_id = 0;
        }
    }
    else if (order.action == 'R') {
        // reset
        reset();
        displayOrder(order, fout, depth);
    }

    if (change) {
        displayOrder(order, fout, depth);
    }
}

void MBPK::reset() {
    bid.clear();
    ask.clear();
    bidq = priority_queue<ll>();
    askq = priority_queue<ll, vector<ll>, greater<ll> >();
    bid_to_order.clear();
    ask_to_order.clear();
    removed_ask.clear();
    removed_bid.clear();
}
