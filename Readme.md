# README

This repository is a minimal C++ quant-trading skeleton (MarketData -> Strategy -> Execution -> Backtest) intended as a demo you can show in interviews. It is single-threaded, easy to understand, and ready to build with CMake.


# Quick build & run (local Ubuntu)
# 1. mkdir build && cd build
# 2. cmake ..
# 3. make
# 4. ./quant_demo ../data/sample.csv

# Notes
# - This skeleton is intentionally simple and readable. In production you'd optimize moving-average with prefix sums,
#   avoid allocations in hot loops, add logging, metrics, unit tests, and handle order/fee/slippage models.
# - If you want, I can split these into separate files and compress into a zip you can download, or push to a GitHub repo template for you.
