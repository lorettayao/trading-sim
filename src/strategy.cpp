
#include "strategy.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class MovingAverageStrategy : public Strategy {
    int short_n_, long_n_;
public:
    MovingAverageStrategy(int s = 5, int l = 20) : short_n_(s), long_n_(l) {}

    std::vector<int> generate_signals(const std::vector<Tick>& ticks) override {
        int n = (int)ticks.size();
        std::vector<int> signals(n, 0);
        if (n == 0) return signals;
        std::vector<double> ma_short(n, 0.0), ma_long(n, 0.0);

        // naive rolling average (clear and easy to read)
        for (int i = 0; i < n; ++i) {
            int sstart = std::max(0, i - short_n_ + 1);
            int lstart = std::max(0, i - long_n_ + 1);
            double ssum = 0.0, lsum = 0.0;
            for (int j = sstart; j <= i; ++j) ssum += ticks[j].price;
            for (int j = lstart; j <= i; ++j) lsum += ticks[j].price;
            ma_short[i] = ssum / (i - sstart + 1);
            ma_long[i] = lsum / (i - lstart + 1);
            if (i >= long_n_ - 1) {
                if (ma_short[i] > ma_long[i]) signals[i] = 1;
                else if (ma_short[i] < ma_long[i]) signals[i] = -1;
            }
        }
        return signals;
    }
};


std::shared_ptr<Strategy> make_moving_average_strategy(int short_window, int long_window) {
    return std::make_shared<MovingAverageStrategy>(short_window, long_window);
}