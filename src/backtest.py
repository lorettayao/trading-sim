# src/backtest.py
import pandas as pd
from strategy import moving_average_strategy

def backtest(df):
    df["return"] = df["close"].pct_change()
    df["strategy_return"] = df["signal"].shift(1) * df["return"]
    cumulative = (1 + df["strategy_return"]).cumprod()
    return cumulative

if __name__ == "__main__":
    df = pd.read_csv("data/btcusdt_1h.csv")
    df = moving_average_strategy(df)
    result = backtest(df)
    print("Final strategy return:", result.iloc[-1])
