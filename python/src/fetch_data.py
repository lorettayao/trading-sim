# src/fetch_data.py
import requests
import pandas as pd

def fetch_binance(symbol="BTCUSDT", interval="1h", limit=1000):
    url = "https://api.binance.com/api/v3/klines"
    params = {"symbol": symbol, "interval": interval, "limit": limit}
    response = requests.get(url, params=params)
    data = response.json()
    
    df = pd.DataFrame(data, columns=[
        "open_time", "open", "high", "low", "close", "volume",
        "close_time", "qav", "num_trades", "taker_base_vol",
        "taker_quote_vol", "ignore"
    ])
    df["open_time"] = pd.to_datetime(df["open_time"], unit="ms")
    df["close"] = df["close"].astype(float)
    return df[["open_time", "close"]]

if __name__ == "__main__":
    df = fetch_binance()
    df.to_csv("data/btcusdt_1h.csv", index=False)
    print("✅ Saved data/btcusdt_1h.csv")
