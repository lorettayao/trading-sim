#include "market_data.hpp"
#include "tick.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>

class CSVMarketData : public MarketData {
    std::vector<Tick> ticks_;
public:
    void load_from_csv(const std::string &path) override {
        ticks_.clear();
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            std::cerr << "Failed to open " << path << std::endl;
            return;
        }
        std::string line;
        // expecting header: timestamp,price
        std::getline(ifs, line);
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string token;
            std::getline(ss, token, ',');
            long long ts = std::stoll(token);
            std::getline(ss, token, ',');
            double price = std::stod(token);
            ticks_.push_back(Tick{ts, price});
        }
    }
    const std::vector<Tick>& get_ticks() const override { return ticks_; }
};

// Factory helper
std::unique_ptr<MarketData> make_csv_market_data() {
    return std::unique_ptr<MarketData>(new CSVMarketData());
}


