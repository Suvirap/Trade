#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

struct Trade {
    int64_t timestamp;      // microseconds since midnight
    string symbol;          // 3 character unique identifier
    int64_t quantity;       // amount traded
    int64_t price;          // price of the trade
};

// Calculate the maximum time gap, total volume, weighted average price and max trade price for each symbol
tuple<int64_t, int64_t, int64_t, int64_t> calculate_statistics(const vector<Trade>& trades) {
    int64_t max_time_gap = 0;
    int64_t total_volume = 0;
    int64_t sum_weighted_prices = 0;
    int64_t max_price = 0;
    int64_t last_timestamp = -1;
    int64_t weighted_average_price = 0;

    for (const auto& trade : trades) {
        if (last_timestamp != -1) {
            max_time_gap = max(max_time_gap, trade.timestamp - last_timestamp);
        }
        last_timestamp = trade.timestamp;

        total_volume += trade.quantity;
        sum_weighted_prices += trade.quantity * trade.price;
        max_price = max(max_price, trade.price);
    }
    weighted_average_price = (total_volume > 0 ? sum_weighted_prices / total_volume : 0);
    
    return make_tuple(max_time_gap, total_volume, weighted_average_price, max_price);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: ./Exercise input.csv output.csv" << endl;
        return 1;
    }
    ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        cerr << "Error: could not open input file" << endl;
        return 1;
    }
    // Read input file and group trades by symbol
    map<string, vector<Trade>> trades_by_symbol;
    string line;
    while (getline(input_file, line)) {
        stringstream ss(line);
        string timestamp_str, symbol, quantity_str, price_str;
        getline(ss, timestamp_str, ',');
        getline(ss, symbol, ',');
        getline(ss, quantity_str, ',');
        getline(ss, price_str, ',');
        Trade trade = {
            stoll(timestamp_str),
            symbol,
            stoll(quantity_str),
            stoll(price_str)
        };
        trades_by_symbol[trade.symbol].emplace_back(trade);
    }

    ofstream output_file(argv[2]);
    if (!output_file.is_open()) {
        cerr << "Error: could not open output file" << endl;
        return 1;
    }

    // Calculate statistics for each symbol and write ouput to file
    for (const auto& [symbol, trades] : trades_by_symbol) {
        auto [max_time_gap, total_volume, weighted_average_price, max_price] = calculate_statistics(trades);
        output_file << symbol << ',' << max_time_gap << ',' << total_volume << ',' << weighted_average_price << ',' << max_price << '\n';
    }

    input_file.close();
    output_file.close();

    return 0;
}