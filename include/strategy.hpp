#pragma once
#include <vector>
#include <memory>
#include "tick.hpp"

// signal: +1 buy/long, 0 no-op, -1 sell/flat
class Strategy {
public:
    virtual std::vector<int> generate_signals(const std::vector<Tick>& ticks) = 0;
    virtual ~Strategy() = default;
};

std::shared_ptr<Strategy> make_moving_average_strategy(int short_window, int long_window);