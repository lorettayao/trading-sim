# Quant Trading Demo 

A simple quantitative trading system built with Python.

## Features
- Fetches real-time market data from Binance API
- Implements a moving-average crossover strategy
- Backtests strategy performance using historical data
- Dockerized for easy deployment

## Run Locally
```bash
git clone https://github.com/<your-username>/quant-trading-demo.git
cd quant-trading-demo
pip install -r requirements.txt
python src/fetch_data.py
python src/backtest.py
