#pragma once
#include <vector>
#include "tick.hpp"

class Execution {
public:
    // returns cumulative return (e.g. 1.05 means +5%)
    virtual double simulate(const std::vector<Tick>& ticks, const std::vector<int>& signals) = 0;
    virtual ~Execution() = default;
};
