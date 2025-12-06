#include <bits/stdc++.h>
// #include "order.h"
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
    ifstream mbp1("mbp.csv");
    ifstream mbp2("mbp10.csv");
    ofstream unequal("unequal.csv");
    Time t;
    t.start();
    // double sum = 0;
    // ll cnt = 0;
    if (mbp1.is_open() && mbp2.is_open()) {
        string row1, row2;
        // Order order1, order2;
        // MBPK mbp10(10);
        int row_no = -1;
        // Order temp;
        while (getline(mbp1, row1) && getline(mbp2, row2)) {
            row1.pop_back();
            // reverse(row2.begin(), row2.end());
            // while (!row2.empty() && row2.back() != ',') {
            //     row2.pop_back();
            // }
            // reverse(row2.begin(), row2.end());
            if (row1 != row2) {
                unequal << row1 << '\n';
                unequal << row2 << "\n\n";
                // unequal << row1.size() << ' ' << row2.size() << '\n';
                // unequal << row1[row1.size() - 1] << '#' << row2[row2.size() - 1] << '\n';
            }
        }
        mbp1.close();
        mbp2.close();
    }
    t.end();
    t.stats();
    return 0;
}

