#pragma once
#include <string>
#include <vector>
#include "tick.hpp"

class MarketData {
public:
    virtual void load_from_csv(const std::string &path) = 0;
    virtual const std::vector<Tick>& get_ticks() const = 0;
    virtual ~MarketData() = default;
};
