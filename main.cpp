#include <bits/stdc++.h>
#include "order.h"
#include <fstream> // for file IO
using namespace std;

#include <chrono>
using namespace std::chrono;
class Time {
private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;
public:

    void start() {
        start_time = high_resolution_clock::now();
    }

    void end() {
        end_time = high_resolution_clock::now();
    }

    double time() {
        auto duration = duration_cast<nanoseconds>(end_time - start_time);
        // double seconds = duration.count() * 1e-9;
        // double milliseconds = duration.count() * 1e-6;
        // double microseconds = duration.count() * 1e-3;
        double nanoseconds = duration.count();

        // cerr << "Elapsed time:\n";
        // cerr << "Seconds: " << seconds << " s\n";
        // cerr << "Milliseconds: " << milliseconds << " ms\n";
        // cerr << "Microseconds: " << microseconds << " µs\n";
        // cerr << "Nanoseconds: " << nanoseconds << " ns\n";

        return nanoseconds;
    }

    void stats() {
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
    Time t;
    t.start();
    // double sum = 0;
    // ll cnt = 0;
    for (int i = 0;i < 10;i++) {
        ifstream mbo("mbo.csv");
        ofstream mbp("mbp10.csv");
        if (mbo.is_open() && mbp.is_open()) {
            mbp << ",ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,depth,price,size,flags,ts_in_delta,sequence,bid_px_00,bid_sz_00,bid_ct_00,ask_px_00,ask_sz_00,ask_ct_00,bid_px_01,bid_sz_01,bid_ct_01,ask_px_01,ask_sz_01,ask_ct_01,bid_px_02,bid_sz_02,bid_ct_02,ask_px_02,ask_sz_02,ask_ct_02,bid_px_03,bid_sz_03,bid_ct_03,ask_px_03,ask_sz_03,ask_ct_03,bid_px_04,bid_sz_04,bid_ct_04,ask_px_04,ask_sz_04,ask_ct_04,bid_px_05,bid_sz_05,bid_ct_05,ask_px_05,ask_sz_05,ask_ct_05,bid_px_06,bid_sz_06,bid_ct_06,ask_px_06,ask_sz_06,ask_ct_06,bid_px_07,bid_sz_07,bid_ct_07,ask_px_07,ask_sz_07,ask_ct_07,bid_px_08,bid_sz_08,bid_ct_08,ask_px_08,ask_sz_08,ask_ct_08,bid_px_09,bid_sz_09,bid_ct_09,ask_px_09,ask_sz_09,ask_ct_09,symbol,order_id\n";
            string row;
            Order order;
            MBPK mbp10(11);
            int row_no = -1;
            Order temp;
            while (getline(mbo, row)) {
                if (row_no == -1) {
                    row_no++;
                    continue;
                }
                order.parse_row(row, row_no);
                order.rtype = 10;
                if (order.action == 'T') {
                    if (order.side == 'N') {
                        mbp10.displayOrder(order, mbp, 0);
                    }
                    else {
                        if (order.side == 'A') {
                            order.side = 'B';
                        }
                        else if (order.side == 'B') {
                            order.side = 'A';
                        }
                        getline(mbo, row); // Fill
                        getline(mbo, row); // Cancel
                        temp.parse_row(row, row_no);
                        order.order_id = temp.order_id;
                        mbp10.processOrder(order, mbp);
                    }
                }
                else {
                    mbp10.processOrder(order, mbp);
                }
                row_no++;
                // sum += t.time();
                // cnt++;
            }
            mbo.close();
        }
    }
    t.end();
    t.stats();
    return 0;
}

