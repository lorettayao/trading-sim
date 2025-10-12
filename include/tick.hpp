#pragma once
#include <cstdint>

struct Tick {
    int64_t timestamp; // unix epoch seconds (or millis if you prefer)
    double price;
};
