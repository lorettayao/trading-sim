#include "execution.hpp"
#include <memory>
#include <vector>

class SimpleExecution : public Execution {
public:
    // assumes signals[i] is the position to hold for period i (1 means long, -1 short, 0 flat)
    double simulate(const std::vector<Tick>& ticks, const std::vector<int>& signals) override {
        int n = (int)ticks.size();
        if (n <= 1) return 1.0;
        double cumulative = 1.0;
        for (int i = 1; i < n; ++i) {
            double ret = (ticks[i].price / ticks[i-1].price) - 1.0; // price return from i-1 -> i
            int pos = signals[i-1]; // use previous signal as current position
            double strat_ret = pos * ret;
            cumulative *= (1.0 + strat_ret);
        }
        return cumulative;
    }
};

std::unique_ptr<Execution> make_simple_execution() {
    return std::unique_ptr<Execution>(new SimpleExecution());
}
