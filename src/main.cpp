#include <iostream>
#include <memory>
#include "market_data.hpp"
#include "strategy.hpp"
#include "execution.hpp"

// forward declarations of helpers in other cpp files
std::unique_ptr<MarketData> make_csv_market_data();
std::unique_ptr<Execution> make_simple_execution();

int main(int argc, char* argv[]) {
    std::string csv_path = "data/sample.csv";
    if (argc > 1) csv_path = argv[1];

    auto md = make_csv_market_data();
    md->load_from_csv(csv_path);
    const auto& ticks = md->get_ticks();

    if (ticks.empty()) {
        std::cerr << "No ticks loaded. Make sure " << csv_path << " exists and has data.\n";
        return 1;
    }

    auto strat = make_moving_average_strategy(5, 20);
    auto signals = strat->generate_signals(ticks);

    auto exec = make_simple_execution();
    double final_ret = exec->simulate(ticks, signals);

    std::cout << "Ticks loaded: " << ticks.size() << "\n";
    std::cout << "Final cumulative return: " << final_ret << "\n";

    // print a few sample rows for demo
    std::cout << "time,price,signal\n";
    for (size_t i = 0; i < ticks.size() && i < 30; ++i) {
        std::cout << ticks[i].timestamp << "," << ticks[i].price << "," << signals[i] << "\n";
    }

    return 0;
}
