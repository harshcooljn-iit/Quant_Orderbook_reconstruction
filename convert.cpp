#include <bits/stdc++.h>
#include "csv.h"
using namespace std;

// (Your Order class remains the same as before)
class Order {
public:
    string ts_recv;
    string ts_event;
    int rtype;
    int publisher_id;
    int instrument_id;
    char action;
    char side;
    double price;
    int size;
    int channel_id; // not needed in mbp
    int order_id; // not needed in mbp
    int flags;
    int ts_delta; // same as ts_in_delta
    int sequence;
    string symbol;

    Order(string ts_recv, string ts_event, int rtype, int publisher_id,
        int instrument_id, char action, char side, double price, int size,
        int channel_id, int order_id, int flags, int ts_delta,
        int sequence, string symbol)
        : ts_recv(ts_recv), ts_event(ts_event), rtype(rtype),
        publisher_id(publisher_id), instrument_id(instrument_id),
        action(action), side(side), price(price), size(size),
        channel_id(channel_id), order_id(order_id), flags(flags),
        ts_delta(ts_delta), sequence(sequence), symbol(symbol) {
    }

    void print() {
        cout << ts_recv << "," << ts_event << "," << rtype << ","
            << publisher_id << "," << instrument_id << "," << action
            << "," << side << "," << price << "," << size << ","
            << channel_id << "," << order_id << "," << flags << ","
            << ts_delta << "," << sequence << "," << symbol << "\n";
    }
};



#include <chrono>
using namespace std::chrono;
class Time {
private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;
public:
    Time() {
        start();
    }

    void start() {
        start_time = high_resolution_clock::now();
    }

    void end() {
        end_time = high_resolution_clock::now();
        time();
    }

    void time() {
        auto duration = duration_cast<nanoseconds>(end_time - start_time);
        double seconds = duration.count() * 1e-9;
        double milliseconds = duration.count() * 1e-6;
        double microseconds = duration.count() * 1e-3;
        double nanoseconds = duration.count();

        cerr << "Elapsed time:\n";
        cerr << "Seconds: " << seconds << " s\n";
        cerr << "Milliseconds: " << milliseconds << " ms\n";
        cerr << "Microseconds: " << microseconds << " µs\n";
        cerr << "Nanoseconds: " << nanoseconds << " ns\n";
    }
};

int main() {
    Time timer;
    {   // File Input-Output , and Fast Input-Output
        freopen("./mbp10.csv", "w", stdout);
        ios_base::sync_with_stdio(0);
        cin.tie(NULL);
        cout.tie(NULL);
    }

    // High-performance parser config (no quotes)
    io::CSVReader<15,
        io::trim_chars<' '>,
        io::no_quote_escape<','>>
        in("./mbo.csv");

    // Read the header line
    in.read_header(io::ignore_missing_column,
        "ts_recv", "ts_event", "rtype", "publisher_id", "instrument_id",
        "action", "side", "price", "size", "channel_id", "order_id",
        "flags", "ts_in_delta", "sequence", "symbol");

    vector<Order> orders;

    // Define variables to read into
    string ts_recv, ts_event, symbol;
    int rtype, publisher_id, instrument_id;
    char action, side;
    double price;
    int size, channel_id, order_id, flags, ts_in_delta, sequence;

    // *** ADDED THIS LINE ***
    // This reads the first data row and discards it.
    in.read_row(ts_recv, ts_event, rtype, publisher_id, instrument_id,
        action, side, price, size, channel_id, order_id,
        flags, ts_in_delta, sequence, symbol);

    // *** LOOP IS UNCHANGED ***
    // This loop will now start on the *second* data row.
    while (in.read_row(ts_recv, ts_event, rtype, publisher_id, instrument_id,
        action, side, price, size, channel_id, order_id,
        flags, ts_in_delta, sequence, symbol)) {

        orders.emplace_back(ts_recv, ts_event, rtype, publisher_id,
            instrument_id, action, side, price, size,
            channel_id, order_id, flags, ts_in_delta,
            sequence, symbol);
    }

    for (auto& order : orders) {
        order.print();
    }
    timer.end();
    return 0;
}

