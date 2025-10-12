# src/strategy.py
import pandas as pd

def moving_average_strategy(df, short=5, long=20):
    df["ma_short"] = df["close"].rolling(short).mean()
    df["ma_long"] = df["close"].rolling(long).mean()
    df["signal"] = 0
    df.loc[df["ma_short"] > df["ma_long"], "signal"] = 1
    df.loc[df["ma_short"] < df["ma_long"], "signal"] = -1
    return df
